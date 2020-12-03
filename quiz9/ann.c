#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct ann;
typedef double (*ann_act_func)(const struct ann *ctx, double a);

typedef struct ann {
    int inputs, hidden_layers, hidden, outputs;
    ann_act_func activation_hidden; /* used for hidden neurons */
    ann_act_func activation_output; /* used for output */
    int total_weights;              /* weights and size of weights buffer */
    int total_neurons; /* neurons + inputs and size of output buffer */
    double *weight;    /* All weights */
    double *output;    /* store input array and output of each neuron */
    double *delta;     /* total_neurons - inputs */
} ann_t;

#define LOOKUP_SIZE 4096

static inline double ann_act_hidden_indirect(const struct ann *ctx, double a)
{
    return ctx->activation_hidden(ctx, a);
}

static inline double ann_act_output_indirect(const struct ann *ctx, double a)
{
    return ctx->activation_output(ctx, a);
}

static const double sigmoid_dom_min = -15.0, sigmoid_dom_max = 15.0;
static double interval, lookup[LOOKUP_SIZE];

#define unlikely(x) __builtin_expect(!!(x), 0)
#define UNUSED __attribute__((unused))

static double ann_act_sigmoid(const ann_t *ctx UNUSED, double a)
{
    if (a < -45.0)
        return 0;
    if (a > 45.0)
        return 1;
    return 1.0 / (1 + exp(-a));
}

static void ann_init_sigmoid_lookup(const ann_t *ctx)
{
    const double f = (sigmoid_dom_max - sigmoid_dom_min) / LOOKUP_SIZE;

    interval = LOOKUP_SIZE / (sigmoid_dom_max - sigmoid_dom_min);
    for (int i = 0; i < LOOKUP_SIZE; ++i)
        lookup[i] = ann_act_sigmoid(ctx, sigmoid_dom_min + f * i);
}

static double ann_act_sigmoid_cached(const ann_t *ctx UNUSED, double a)
{
    assert(!isnan(a));

    if (a < sigmoid_dom_min)
        return lookup[0];
    if (a >= sigmoid_dom_max)
        return lookup[LOOKUP_SIZE - 1];

    size_t j = (a - sigmoid_dom_min) * interval + 0.5;
    if (unlikely(j >= LOOKUP_SIZE))
        return lookup[LOOKUP_SIZE - 1];

    return lookup[j];
}

/* conventional generator (splitmix) developed by Steele et al. */
static uint64_t splitmix_x;
static inline uint64_t splitmix()
{
    splitmix_x += 0x9E3779B97F4A7C15;
    uint64_t z = splitmix_x;
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EB;
    return z ^ (z >> 31);
}

static void splitmix_init()
{
    /* Hopefully, ASLR makes our function address random */
    uintptr_t x = (uintptr_t)((void *) &splitmix_init);
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    x ^= (uintptr_t) time.tv_sec;
    x ^= (uintptr_t) time.tv_nsec;
    splitmix_x = x;

    /* do a few randomization steps */
    uintptr_t max = ((x ^ (x >> 17)) & 0x0F) + 1;
    for (uintptr_t i = 0; i < max; i++)
        splitmix();
}

/* uniform random numbers between 0 and 1 */
#define ANN_RAND() (((double) splitmix()) / UINT64_MAX)

/* Set weights randomly */
void ann_randomize(ann_t *ctx)
{
    for (int i = 0; i < ctx->total_weights; ++i)
        ctx->weight[i] = ANN_RAND() - 0.5; /* weights from -0.5 to 0.5 */
}

/* Create and return a new network context */
ann_t *ann_init(int inputs, int hidden_layers, int hidden, int outputs)
{
    if (hidden_layers < 0)
        return 0;
    if (inputs < 1)
        return 0;
    if (outputs < 1)
        return 0;
    if (hidden_layers > 0 && hidden < 1)
        return 0;

    const int hidden_weights =
        hidden_layers ? (inputs + 1) * hidden +
                            (hidden_layers - 1) * (hidden + 1) * hidden
                      : 0;
    const int output_weights =
        (hidden_layers ? (hidden + 1) : (inputs + 1)) * outputs;
    const int total_weights = (hidden_weights + output_weights);

    const int total_neurons = (inputs + hidden * hidden_layers + outputs);

    /* Allocate extra size for weights, outputs, and deltas. */
    const int size =
        sizeof(ann_t) + sizeof(double) * (total_weights + total_neurons +
                                          (total_neurons - inputs));
    ann_t *ret = malloc(size);
    assert(ret);

    ret->inputs = inputs;
    ret->hidden_layers = hidden_layers;
    ret->hidden = hidden;
    ret->outputs = outputs;

    ret->total_weights = total_weights;
    ret->total_neurons = total_neurons;

    ret->weight = (double *) ((char *) ret + sizeof(ann_t));
    ret->output = ret->weight + ret->total_weights;
    ret->delta = ret->output + ret->total_neurons;

    ann_randomize(ret);

    ret->activation_hidden = ann_act_sigmoid_cached;
    ret->activation_output = ann_act_sigmoid_cached;

    ann_init_sigmoid_lookup(ret);
    return ret;
}

/* Return a new copy of network context */
ann_t *ann_copy(ann_t const *ctx)
{
    const int size = sizeof(ann_t) +
                     sizeof(double) * (ctx->total_weights + ctx->total_neurons +
                                       (ctx->total_neurons - ctx->inputs));
    ann_t *ret = malloc(size);
    assert(ret);

    memcpy(ret, ctx, size);

    ret->weight = (double *) ((char *) ret + sizeof(ann_t));
    ret->output = ret->weight + ret->total_weights;
    ret->delta = ret->output + ret->total_neurons;

    return ret;
}

/* Free the memory used by a network context */
void ann_free(ann_t *ctx)
{
    free(ctx);
}

/* Run the feedforward algorithm to calculate the output of the network. */
double const *ann_run(ann_t const *ctx, double const *inputs)
{
    double const *w = ctx->weight;
    double *o = ctx->output + ctx->inputs;
    double const *i = ctx->output;

    /* Copy the inputs to the scratch area, where we also store each neuron's
     * output, for consistency. This way the first layer isn't a special case.
     */
    memcpy(ctx->output, inputs, sizeof(double) * ctx->inputs);

    if (!ctx->hidden_layers) {
        double *ret = o;
        for (int j = 0; j < ctx->outputs; ++j) {
            double sum = *w++ * (-1.0);
            for (int k = 0; k < ctx->inputs; ++k)
                sum += *w++ * i[k];
            *o++ = ann_act_output_indirect(ctx, sum);
        }
        return ret;
    }

    /* Figure input layer */
    for (int j = 0; j < ctx->hidden; ++j) {
        double sum = *w++ * (-1.0);
        for (int k = 0; k < ctx->inputs; ++k)
            sum += *w++ * i[k];
        *o++ = ann_act_hidden_indirect(ctx, sum);
    }

    i += ctx->inputs;

    /* Figure hidden layers, if any. */
    for (int h = 1; h < ctx->hidden_layers; ++h) {
        for (int j = 0; j < ctx->hidden; ++j) {
            double sum = *w++ * (-1.0);
            for (int k = 0; k < ctx->hidden; ++k)
                sum += *w++ * i[k];
            *o++ = ann_act_hidden_indirect(ctx, sum);
        }
        i += ctx->hidden;
    }

    double const *ret = o;

    /* Figure output layer. */
    for (int j = 0; j < ctx->outputs; ++j) {
        double sum = *w++ * (-1.0);
        for (int k = 0; k < ctx->hidden; ++k)
            sum += *w++ * i[k];
        *o++ = ann_act_output_indirect(ctx, sum);
    }

    assert(w - ctx->weight == ctx->total_weights);
    assert(o - ctx->output == ctx->total_neurons);

    return ret;
}

#define LOOP_MAX 1024

int main(int argc, char *argv[])
{
    splitmix_init();

    /* Input and expected out data for the 2-input Multiplexer */
    const double input[8][3] = {
        {0, 0, 0}, {0, 0, 1}, {0, 1, 0}, {0, 1, 1},
        {1, 0, 0}, {1, 0, 1}, {1, 1, 0}, {1, 1, 1},
    };
    const double output[8] = {
        0, 0, 1, 1, 0, 1, 0, 1,
    };

    /* New network with 3 inputs, 1 hidden layer of 2 neurons,
     * and 1 output.
     */
    ann_t *ctx = ann_init(3, 1, 2, 1);

    double err, last_err = LOOP_MAX;
    int count = 0;
    do {
        ++count;
        if (count % LOOP_MAX == 0) {
            ann_randomize(ctx);
            last_err = LOOP_MAX;
        }

        ann_t *save = ann_copy(ctx);

        /* Take a random guess at the ANN weights. */
        for (int i = 0; i < ctx->total_weights; ++i)
            ctx->weight[i] += ANN_RAND() - 0.5;

        err = 0;
        for (int k = 0; k < (1 << 3); k++)
            err += pow(*ann_run(ctx, input[k]) - output[k], 2.0);

        /* Keep these weights if they're an improvement. */
        if (err < last_err) {
            ann_free(save);
            last_err = err;
        } else {
            ann_free(ctx);
            ctx = save;
        }
    } while (err > 0.01);
    printf("Finished in %d loops.\n", count);

    /* Run the network and see what it predicts. */
    for (int k = 0; k < (1 << 3); k++)
        printf("[%1.f, %1.f, %1.f] = %1.f.\n", input[k][0], input[k][1],
               input[k][2], *ann_run(ctx, input[k]));

    ann_free(ctx);
    return 0;
}
