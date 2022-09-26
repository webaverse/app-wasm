#ifndef _CUBIC_BEZIER_EASING_H
#define _CUBIC_BEZIER_EASING_H
// #pragma once
namespace CubicBezierEasing {

  /**
  * https://github.com/gre/bezier-easing
  * BezierEasing - use bezier curve for transition easing function
  * by Gaëtan Renaudeau 2014 - 2015 – MIT License
  */

  // These values are established by empiricism with tests (tradeoff: performance VS precision)
  unsigned int NEWTON_ITERATIONS = 4;
  float NEWTON_MIN_SLOPE = 0.001;
  float SUBDIVISION_PRECISION = 0.0000001;
  unsigned int SUBDIVISION_MAX_ITERATIONS = 10;

  unsigned int kSplineTableSize = 11;
  float kSampleStepSize = 1.0 / (kSplineTableSize - 1.0);

  // cache init() reaults
  float _mX1, _mX2, _mY1, _mY2;
  float *sampleValues;
  bool isLinearEasing;
  // end: cache init() reaults

  float A (float aA1, float aA2) { return 1.0 - 3.0 * aA2 + 3.0 * aA1; }
  float B (float aA1, float aA2) { return 3.0 * aA2 - 6.0 * aA1; }
  float C (float aA1)      { return 3.0 * aA1; }

  // Returns x(t) given t, x1, and x2, or y(t) given t, y1, and y2.
  float calcBezier (float aT, float aA1, float aA2) { return ((A(aA1, aA2) * aT + B(aA1, aA2)) * aT + C(aA1)) * aT; }

  // Returns dx/dt given t, x1, and x2, or dy/dt given t, y1, and y2.
  float getSlope (float aT, float aA1, float aA2) { return 3.0 * A(aA1, aA2) * aT * aT + 2.0 * B(aA1, aA2) * aT + C(aA1); }

  float myAbs(float x) { // todo: use native `abs()`.
    if (x >= 0) {
      return x;
    } else {
      return -x;
    }
  }

  float binarySubdivide (float aX, float aA, float aB, float mX1, float mX2) {
    float currentX, currentT, i = 0;
    do {
      currentT = aA + (aB - aA) / 2.0;
      currentX = calcBezier(currentT, mX1, mX2) - aX;
      if (currentX > 0.0) {
        aB = currentT;
      } else {
        aA = currentT;
      }
    } while (myAbs(currentX) > SUBDIVISION_PRECISION && ++i < SUBDIVISION_MAX_ITERATIONS);
    return currentT;
  }

  float newtonRaphsonIterate (float aX, float aGuessT, float mX1, float mX2) {
    for (unsigned int i = 0; i < NEWTON_ITERATIONS; ++i) {
      float currentSlope = getSlope(aGuessT, mX1, mX2);
      if (currentSlope == 0.0) {
        return aGuessT;
      }
      float currentX = calcBezier(aGuessT, mX1, mX2) - aX;
      aGuessT -= currentX / currentSlope;
    }
    return aGuessT;
  }

  float LinearEasing (float x) {
    return x;
  }

  float getTForX (float aX) {
    float intervalStart = 0.0;
    unsigned int currentSample = 1;
    unsigned int lastSample = kSplineTableSize - 1;
    std::cout << "-wasm- aX: " << aX << std::endl;
    std::cout << "-wasm- currentSample: " << currentSample << std::endl;
    std::cout << "-wasm- lastSample: " << lastSample << std::endl;

    for (; currentSample != lastSample && sampleValues[currentSample] <= aX; ++currentSample) {
      std::cout << "-wasm- currentSample: " << currentSample << std::endl;
      std::cout << "-wasm- sampleValues[currentSample]: " << sampleValues[currentSample] << std::endl;
      std::cout << "-wasm- intervalStart: " << intervalStart << std::endl;
      intervalStart += kSampleStepSize;
    }
    --currentSample;

    // Interpolate to provide an initial guess for t
    float dist = (aX - sampleValues[currentSample]) / (sampleValues[currentSample + 1] - sampleValues[currentSample]);
    float guessForT = intervalStart + dist * kSampleStepSize;

    float initialSlope = getSlope(guessForT, _mX1, _mX2);
    // std::cout << "-wasm- currentSample: " << currentSample << std::endl;
    // std::cout << "-wasm- dist: " << dist << std::endl;
    // std::cout << "-wasm- guessForT: " << guessForT << std::endl;
    // std::cout << "-wasm- initialSlope: " << initialSlope << std::endl;
    if (initialSlope >= NEWTON_MIN_SLOPE) {
      // std::cout << "-wasm- case 1" << std::endl;
      return newtonRaphsonIterate(aX, guessForT, _mX1, _mX2);
    } else if (initialSlope == 0.0) {
      // std::cout << "-wasm- case 2" << std::endl;
      return guessForT;
    } else {
      // std::cout << "-wasm- case 3" << std::endl;
      return binarySubdivide(aX, intervalStart, intervalStart + kSampleStepSize, _mX1, _mX2);
    }
  }

  float cubicBezier(float x) {
    if (isLinearEasing) {
      return LinearEasing(x);
    } else {
      // Because JavaScript number are imprecise, we should guarantee the extremes are right.
      if (x == 0 || x == 1) { // Keep this check to prevent values inputed from js are imprecise ?
        return x;
      }
      float t = getTForX(x);
      std::cout << "-wasm- t: " << t << std::endl;
      return calcBezier(t, _mY1, _mY2);
    }
  }

  void init(float mX1, float mY1, float mX2, float mY2) {
    if (!(0 <= mX1 && mX1 <= 1 && 0 <= mX2 && mX2 <= 1)) {
      std::cerr << "bezier x values must be in [0, 1] range" << std::endl;
    }

    if (mX1 == mY1 && mX2 == mY2) {
      isLinearEasing = true;
      return;
    } else {
      isLinearEasing = false;
    }

    _mX1 = mX1;
    _mX2 = mX2;
    _mY1 = mY1;
    _mY2 = mY2;

    // Precompute samples table
    sampleValues = new float[kSplineTableSize]; // todo: destory ?
    for (unsigned int i = 0; i < kSplineTableSize; ++i) {
      sampleValues[i] = calcBezier(i * kSampleStepSize, mX1, mX2);
      std::cout << "sampleValues " << i << " : " << sampleValues[i] << std::endl;
    }
  };

};
#endif // _CUBIC_BEZIER_EASING_H