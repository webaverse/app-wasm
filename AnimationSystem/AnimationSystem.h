#ifndef _ANIMATIONSYSTEM_H
#define _ANIMATIONSYSTEM_H
#include "physics.h"
#include <iostream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;


  // --- actionInterpolants
  class ScalarInterpolant {
  public:
    bool evaluatee;
    float value;
    float minValue;
    float maxValue;
    ScalarInterpolant(float minValue, float maxValue) {
      this->value = minValue;
      this->minValue = minValue;
      this->maxValue = maxValue;
    }
    virtual float get() {
      return this->value;
    }
    virtual float getNormalized() {
      return this->value / (this->maxValue - this->minValue);
    }
    virtual float getInverse() {
      return this->maxValue - this->value;
    }
    virtual void update(float timeDiff, bool evaluatee) { }
  };
  class BiActionInterpolant: public ScalarInterpolant {
  public:
    using ScalarInterpolant::ScalarInterpolant;
    void update(float timeDiff, bool evaluatee) {
      this->value += (evaluatee ? 1 : -1) * timeDiff;
      this->value = fmin(fmax(this->value, this->minValue), this->maxValue);
    }
  };
  class UniActionInterpolant: public ScalarInterpolant {
  public:
    using ScalarInterpolant::ScalarInterpolant;
    void update(float timeDiff, bool evaluatee) {
      if (evaluatee) {
        this->value += timeDiff;
        this->value = fmin(fmax(this->value, this->minValue), this->maxValue);
      } else {
        this->value = this->minValue;
      }
    }
  };
  class InfiniteActionInterpolant: public ScalarInterpolant {
  public:
    InfiniteActionInterpolant(float minValue): ScalarInterpolant(minValue, std::numeric_limits<float>::infinity()) { }
    void update(float timeDiff, bool evaluatee) {
      if (evaluatee) {
        this->value += timeDiff;
      } else {
        this->value = this->minValue;
      }
    }
  };
  // --- End: actionInterpolants

namespace AnimationSystem {
  struct Interpolant;
  struct Animation;
  struct AnimationMapping;
  class Avatar;
  class AnimationNode;
  class AnimationMixer;

  struct Interpolant {
    unsigned int numParameterPositions;
    float *parameterPositions;
    // float *resultBuffer;
    float resultBuffer[4];
    /* AnimationSystem/AnimationSystem.cc:630:12: warning: address of stack memory associated with local variable 'interpolant' returned [-Wreturn-stack-address]
    return interpolant.resultBuffer;
           ^~~~~~~~~~~ */
    unsigned int numSampleValues;
    float *sampleValues;
    unsigned int valueSize;
  };
  struct Animation {
    float duration;
    std::vector<Interpolant *> interpolants; // todo: pure array?
    unsigned int currentInterpolantIndex = 0;
    unsigned int index;
    std::string name;
  };
  struct AnimationMapping { // spec
    // float *dst =  (float *)malloc((4) * sizeof(float));
    // float *dst = new float[4];
    float dst[4];
    bool isPosition;
    unsigned int index;
    std::string boneName;
    bool isTop;
    bool isArm;
  };

  class Avatar {
  public:

    std::unordered_map<std::string, json> actions;
    std::unordered_map<std::string, ScalarInterpolant *> actionInterpolants;

    AnimationMixer *mixer;

    // values
    float activateTime;
    float landTime;
    float fallLoopFactor;
    float fallLoopTime;
    float flyTime;
    float doubleJumpTime;
    float jumpTime;
    float narutoRunTime;
    // float narutoRunTimeFactor;
    float danceFactor;
    // float crouchMaxTime;
    float emoteFactor;
    float lastEmoteTime; // todo: calc on wasm side.
    float idleWalkFactor; // todo: calc on wasm side?
    float useTime;
    float useAnimationEnvelopeLength; // todo: calc on wasm side.
    float hurtTime;
    float unuseTime;
    float aimTime;
    float aimMaxTime;
    float walkRunFactor; // todo: calc on wasm side?
    float crouchFactor;
    float pickUpTime;
    float forwardFactor; // todo: calc on wasm side.
    float backwardFactor; // todo: calc on wasm side.
    float leftFactor; // todo: calc on wasm side.
    float rightFactor; // todo: calc on wasm side.
    float mirrorLeftFactorReverse; // todo: calc on wasm side.
    float mirrorLeftFactor; // todo: calc on wasm side.
    float mirrorRightFactorReverse; // todo: calc on wasm side.
    float mirrorRightFactor; // todo: calc on wasm side.
    float landTimeS; // todo: calc on wasm side.
    float timeSinceLastMoveS;
    float swimTime;
    float movementsTime;
    float sprintFactor;
    float movementsTransitionFactor;

    // states
    bool jumpState;
    // int jumpActionsCount = 0;
    bool doubleJumpState;
    // int doubleJumpActionsCount = 0;
    bool flyState;
    // int flyActionsCount = 0;
    bool crouchState;
    // int crouchActionsCount = 0;
    bool narutoRunState;
    // int narutoRunActionsCount = 0;
    bool sitState;
    // int sitActionsCount = 0;
    bool holdState;
    // int holdActionsCount = 0;
    bool pickUpState;
    // int pickUpActionsCount = 0;
    bool swimState;
    // int swimActionsCount = 0;
    bool activateState;
    // int activateActionsCount = 0;
    bool useState;
    // int useActionsCount = 0;
    bool aimState;
    // int aimActionsCount = 0;
    bool fallLoopState;
    // int fallLoopActionsCount = 0;
    bool danceState;
    // int danceActionsCount = 0;
    bool emoteState;
    // int emoteActionsCount = 0;
    bool hurtState;
    // int hurtActionsCount = 0;

    //
    bool landWithMoving;
    bool fallLoopFromJump;

    int activateAnimationIndex;
    int sitAnimationIndex;
    int danceAnimationIndex;
    int emoteAnimationIndex;
    int useAnimationIndex;
    int useAnimationComboIndex;
    int hurtAnimationIndex;
    int unuseAnimationIndex;
    int aimAnimationIndex;

    std::vector<int> useAnimationEnvelopeIndices;

    //

    std::string testBlendStrings; // test
    
    void update(float *scratchStack, float timeDiff);
    void addAction(char *scratchStack, unsigned int stringByteLength);
    void removeAction(char *scratchStack, unsigned int stringByteLength);
    void testLogActions();
    float getActionInterpolant(char *scratchStack, unsigned int stringByteLength, unsigned int type = 0); // 0: get(), 1: getNormalized(), 2: getInverse()
  };
  class AnimationMixer {
  public:
    // static float now;
    static float nowS;

    Avatar *avatar;
    // float animationValues[53][4];
    // float animationValues[212];
    float *animationValues;

    float *update(float now, float nowS);
  };

  // ------
  // need run in this order
  void createAnimationMapping(bool isPosition, unsigned int index, bool isTop, bool isArm, char *scratchStack, unsigned int nameByteLength);
  Animation *createAnimation(char *scratchStack, unsigned int nameByteLength, float duration);
  void createAnimationInterpolant(Animation *animation, unsigned int numParameterPositions, float *parameterPositions, unsigned int numSampleValues, float *sampleValues, unsigned int valueSize);
  unsigned int initAnimationSystem(char *scratchStack);
  AnimationMixer *createAnimationMixer();
  Avatar *createAnimationAvatar(AnimationMixer *mixer);
  // end: need run in this order
  // --- Interpolators
  // --- End: Interpolators
};

#endif // _ANIMATIONSYSTEM_H
