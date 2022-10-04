#ifndef _ANIMATIONSYSTEM_H
#define _ANIMATIONSYSTEM_H
#include "physics.h"
#include <iostream>

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
    float narutoRunTimeFactor;
    float danceFactor;
    float crouchMaxTime;
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

    bool landWithMoving;
    bool flyState;
    bool doubleJumpState;
    bool jumpState;
    bool sitState;
    bool narutoRunState;
    bool holdState;
    bool pickUpState;
    bool swimState;
    //
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
    
    void update(float *scratchStack);
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
};

#endif // _ANIMATIONSYSTEM_H
