#include "uc_lang_OSClock.h"

#ifdef _WIN32
#include <windows.h>
#include <stdint.h>
#else
#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#endif

/**
 * Returns unsigned millis since epoch, like System.currentTimeMillis but always non-negative.
 */
JNIEXPORT jlong JNICALL Java_uc_lang_OSClock_currentTimeMillis(JNIEnv *env, jclass cls) {
#ifdef _WIN32
	FILETIME ft;
	ULARGE_INTEGER li;
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	uint64_t t = li.QuadPart / 10000ULL - 11644473600000ULL;
	return (jlong)t;
#else
	struct timeval tv;
	gettimeofday(&tv, 0);

	uint64_t t = (uint64_t)tv.tv_sec * 1000ULL + tv.tv_usec / 1000U;
	return (jlong)t;
#endif
}

/**
 * Returns signed nanoseconds from some arbitrary point, like System.nanoTime().
 */
JNIEXPORT jlong JNICALL Java_uc_lang_OSClock_nanoTime(JNIEnv *env, jclass cls) {
#ifdef _WIN32
	LARGE_INTEGER freq, counter;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&counter);

	jlong seconds = counter.QuadPart / freq.QuadPart;
	jlong remainder_ticks = counter.QuadPart % freq.QuadPart;
	jlong nanos = (remainder_ticks * 1000000000LL) / freq.QuadPart;

	jlong t = seconds * 1000000000LL + nanos;
	return t;
#else
	struct timespec ts;
	#ifdef CLOCK_MONOTONIC
	clock_gettime(CLOCK_MONOTONIC, &ts);
	#else
	clock_gettime(CLOCK_REALTIME, &ts);
	#endif

	jlong t = (jlong)ts.tv_sec * 1000000000LL + ts.tv_nsec;
	return t;
#endif
}

/**
 * Fast, economical version of epoch millis (may be coarser/less accurate).
 */
JNIEXPORT jlong JNICALL Java_uc_lang_OSClock_getEpochMillis(JNIEnv *env, jclass cls) {
#ifdef _WIN32
	FILETIME ft;
	ULARGE_INTEGER li;
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	uint64_t t = li.QuadPart / 10000ULL - 11644473600000ULL;
	return (jlong)t;
#else
	#if defined(CLOCK_REALTIME_COARSE)
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME_COARSE, &ts);

	uint64_t t = (uint64_t)ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
	return (jlong)t;
	#else
	struct timeval tv;
	gettimeofday(&tv, 0);

	uint64_t t = (uint64_t)tv.tv_sec * 1000ULL + tv.tv_usec / 1000U;
	return (jlong)t;
	#endif
#endif
}

/**
 * Returns unsigned seconds since epoch.
 */
JNIEXPORT jlong JNICALL Java_uc_lang_OSClock_getEpochSecond(JNIEnv *env, jclass cls) {
#ifdef _WIN32
	FILETIME ft;
	ULARGE_INTEGER li;
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	uint64_t t = li.QuadPart / 10000000ULL - 11644473600ULL;
	return (jlong)t;
#else
	struct timeval tv;
	gettimeofday(&tv, 0);

	uint64_t t = (uint64_t)tv.tv_sec;
	return (jlong)t;
#endif
}