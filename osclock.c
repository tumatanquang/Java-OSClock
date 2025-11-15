#include "uc_java_OSClock.h"
#include <stdint.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
static double nanos_per_count;
#else
#include <sys/time.h>
#endif

/**
 * Initialize Windows performance counter at load time
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
#if defined _WIN32
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	nanos_per_count = 1.0e9 / (double)freq.QuadPart;
#endif
	return JNI_VERSION_1_2;
}

/**
 * Returns unsigned millis since epoch, like System.currentTimeMillis() but always non-negative
 */
JNIEXPORT jlong JNICALL Java_uc_java_OSClock_currentTimeMillis(JNIEnv *env, jclass cls) {
#if defined _WIN32
	FILETIME ft;
	ULARGE_INTEGER li;
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	uint64_t t = li.QuadPart / 10000ULL - 11644473600000ULL;
	return (jlong)t;
#elif defined CLOCK_REALTIME
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);

	uint64_t t = (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)ts.tv_nsec / 1000000ULL;
	return (jlong)t;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);

	uint64_t t = (uint64_t)tv.tv_sec * 1000ULL + (uint64_t)tv.tv_usec / 1000U;
	return (jlong)t;
#endif
}

/**
 * Returns signed nanoseconds from some arbitrary point, like System.nanoTime()
 */
JNIEXPORT jlong JNICALL Java_uc_java_OSClock_nanoTime(JNIEnv *env, jclass cls) {
#if defined _WIN32
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);

	jlong t = (jlong)((double)counter.QuadPart * nanos_per_count);
	return t;
#elif defined CLOCK_MONOTONIC
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);

	jlong t = (jlong)ts.tv_sec * 1000000000LL + (jlong)ts.tv_nsec;
	return t;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);

	jlong t = (jlong)tv.tv_sec * 1000000000LL + (jlong)tv.tv_usec * 1000LL;
	return t;
#endif
}

/**
 * High performance, fast, save but less accurate version of currentTimeMillis()
 */
JNIEXPORT jlong JNICALL Java_uc_java_OSClock_getEpochMillis(JNIEnv *env, jclass cls) {
#if defined _WIN32
	FILETIME ft;
	ULARGE_INTEGER li;
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	uint64_t t = li.QuadPart / 10000ULL - 11644473600000ULL;
	return (jlong)t;
#elif defined CLOCK_REALTIME_COARSE
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME_COARSE, &ts);

	uint64_t t = (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)ts.tv_nsec / 1000000ULL;
	return (jlong)t;
#elif defined CLOCK_REALTIME
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);

	uint64_t t = (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)ts.tv_nsec / 1000000ULL;
	return (jlong)t;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);

	uint64_t t = (uint64_t)tv.tv_sec * 1000ULL + (uint64_t)tv.tv_usec / 1000U;
	return (jlong)t;
#endif
}

/**
 * Returns seconds since epoch
 */
JNIEXPORT jlong JNICALL Java_uc_java_OSClock_getEpochSecond(JNIEnv *env, jclass cls) {
	return (jlong)time(NULL);
}