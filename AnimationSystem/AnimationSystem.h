#ifndef _ANIMATIONSYSTEM_H
#define _ANIMATIONSYSTEM_H
#include "physics.h"
#include <iostream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;


  // --- actionInterpolants
  class BiActionInterpolant {
  public:
    float value;
    float minValue;
    float maxValue;
    BiActionInterpolant(float minValue, float maxValue) {
      this->value = minValue;
      this->minValue = minValue;
      this->maxValue = maxValue;
    }
    float get() {
      return this->value;
    }
    float getNormalized() {
      return this->value / (this->maxValue - this->minValue);
    }
    float getInverse() {
      return this->maxValue - this->value;
    }
    void update(float timeDiff, bool evaluatee) {
      this->value += (evaluatee ? 1 : -1) * timeDiff;
      this->value = fmin(fmax(this->value, this->minValue), this->maxValue);
    }
  };
  class UniActionInterpolant {
  public:
    float value;
    float minValue;
    float maxValue;
    UniActionInterpolant(float minValue, float maxValue) {
      this->value = minValue;
      this->minValue = minValue;
      this->maxValue = maxValue;
    }
    float get() {
      return this->value;
    }
    float getNormalized() {
      return this->value / (this->maxValue - this->minValue);
    }
    float getInverse() {
      return this->maxValue - this->value;
    }
    void update(float timeDiff, bool evaluatee) {
      if (evaluatee) {
        this->value += timeDiff;
        this->value = fmin(fmax(this->value, this->minValue), this->maxValue);
      } else {
        this->value = this->minValue;
      }
    }
  };
  class InfiniteActionInterpolant {
  public:
    float value;
    float minValue;
    float maxValue;
    InfiniteActionInterpolant(float minValue) {
      this->value = minValue;
      this->minValue = minValue;
      this->maxValue = std::numeric_limits<float>::infinity();
    }
    float get() {
      return this->value;
    }
    float getNormalized() {
      return this->value / (this->maxValue - this->minValue);
    }
    float getInverse() {
      return this->maxValue - this->value;
    }
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
    float resultBuffer[4];
    unsigned int numSampleValues;
    float *sampleValues;
    unsigned int valueSize;
  };
  struct Animation {
    float duration;
    std::vector<Interpolant *> interpolants;
    unsigned int currentInterpolantIndex = 0;
    unsigned int index;
    std::string name;
  };
  struct AnimationMapping { // spec
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

    // ActionInterpolants
    BiActionInterpolant *crouchActI;
    UniActionInterpolant *activateActI;
    InfiniteActionInterpolant *useActI;
    InfiniteActionInterpolant *pickUpActI;
    InfiniteActionInterpolant *unuseActI;
    InfiniteActionInterpolant *aimActI;
    InfiniteActionInterpolant *narutoRunActI;
    InfiniteActionInterpolant *flyActI;
    InfiniteActionInterpolant *swimActI;
    InfiniteActionInterpolant *jumpActI;
    InfiniteActionInterpolant *doubleJumpActI;
    InfiniteActionInterpolant *landActI;
    BiActionInterpolant *danceActI;
    BiActionInterpolant *emoteActI;
    InfiniteActionInterpolant *fallLoopActI;
    BiActionInterpolant *fallLoopTransitionActI;
    InfiniteActionInterpolant *hurtActI;

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
    float lastEmoteTime;
    float idleWalkFactor;
    float useTime;
    float useAnimationEnvelopeLength;
    float hurtTime;
    float unuseTime;
    float aimTime;
    float aimMaxTime;
    float walkRunFactor;
    float crouchFactor;
    float pickUpTime;
    float forwardFactor;
    float backwardFactor;
    float leftFactor;
    float rightFactor;
    float mirrorLeftFactorReverse;
    float mirrorLeftFactor;
    float mirrorRightFactorReverse;
    float mirrorRightFactor;
    float landTimeS;
    float timeSinceLastMoveS;
    float swimTime;
    float movementsTime;
    float sprintFactor;
    float movementsTransitionFactor;

    // states
    bool jumpState;
    bool doubleJumpState;
    bool flyState;
    bool crouchState;
    bool narutoRunState;
    bool sitState;
    bool holdState;
    bool pickUpState;
    bool swimState;
    bool activateState;
    bool useState;
    bool aimState;
    bool fallLoopState;
    bool danceState;
    bool emoteState;
    bool hurtState;

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
    
    void update(float *scratchStack, float timeDiff);
    void addAction(char *scratchStack, unsigned int stringByteLength);
    void removeAction(char *scratchStack, unsigned int stringByteLength);
  };
  class AnimationMixer {
  public:
    // static float now;
    static float nowS;

    Avatar *avatar;
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
};

#endif // _ANIMATIONSYSTEM_H
