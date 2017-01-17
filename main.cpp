#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

volatile double FINAL_RESULT = 0.0;
pthread_mutex_t mutex;

struct trapezoid_data {
    double a;
    double b;
    double step;
};

void *calculate_trapezoid(void *args);

double f(double x) {
    return x;
}

int main(int argc, char *argv[]) {

    double A = 0;
    double B = 10;

    int THREADS_COUNT = atoi(argv[1]);
    int STEP_COUNT = atoi(argv[2]);

    if (THREADS_COUNT > STEP_COUNT) {
        THREADS_COUNT = STEP_COUNT;
    }

    double STEP = (B - A) / (double) STEP_COUNT;

    int steps_per_thread = STEP_COUNT / THREADS_COUNT;
    int threads_with_additional_step = STEP_COUNT % THREADS_COUNT;

    if (pthread_mutex_init(&mutex, NULL)) {
        perror("pthread_mutex_init");
        return 1;
    }

    pthread_t threads[THREADS_COUNT];
    struct trapezoid_data thapezoid_args[THREADS_COUNT];

    double thread_a;
    double thread_b = A;

    for (int i = 0; i < THREADS_COUNT; i++) {

        thread_a = thread_b;

        if (i == THREADS_COUNT - 1) {
            thread_b = B;
        } else {
            thread_b = thread_a + STEP * steps_per_thread;
            if (threads_with_additional_step != 0) {
                thread_b += STEP;
                threads_with_additional_step--;
            }
        }

        thapezoid_args[i].a = thread_a;
        thapezoid_args[i].b = thread_b;
        thapezoid_args[i].step = STEP;

        if (pthread_create(&threads[i], NULL, calculate_trapezoid, &thapezoid_args[i])) {
            perror("pthread_create");
            return 1;
        }
    }

    for (int i = 0; i < THREADS_COUNT; ++i) {
        if (pthread_join(threads[i], NULL)) {
            perror("pthread_join");
            return 1;
        }
    }

    if (pthread_mutex_destroy(&mutex)) {
        perror("pthread_mutex_destroy");
        return 1;
    }

    printf("FINAL_RESULT: %f\n", FINAL_RESULT);

    return 0;
}

void *calculate_trapezoid(void *args) {

    struct trapezoid_data *data = (struct trapezoid_data *) args;
    double a = data->a;
    double b = data->b;
    double step = data->step;

    double result = 0.0;

    double f_a = f(a);

    double f_b;
    double b_i = a + step;

    while (b_i <= b) {
        f_b = f(b_i);
        result += (f_a + f_b);

        f_a = f_b;

        b_i += step;
    }

    result = result * step / 2;

    b_i -= step;
    if (b - b_i > 0) {
        result += (f_a + f(b)) * (b - b_i) / 2;
    }

    if (pthread_mutex_lock(&mutex)) {
        perror("pthread_mutex_lock");
    }

    FINAL_RESULT += result;

    if (pthread_mutex_unlock(&mutex)) {
        perror("pthread_mutex_unlock");
    }

    pthread_exit(NULL);
}

