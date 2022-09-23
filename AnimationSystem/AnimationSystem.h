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
  
  enum BoneIndex {
    // Hips = 0, // position
    Hips = 1, // quaternion
    Spine = 2,
    Chest = 3,
    UpperChest = 4,
    Neck = 5,
    Head = 6,
    Left_shoulder = 7,
    Left_arm = 8,
    Left_elbow = 9,
    Left_wrist = 10,
    Left_middleFinger1 = 11,
    Left_middleFinger2 = 12,
    Left_middleFinger3 = 13,
    Left_thumb0 = 14,
    Left_thumb1 = 15,
    Left_thumb2 = 16,
    Left_indexFinger1 = 17,
    Left_indexFinger2 = 18,
    Left_indexFinger3 = 19,
    Left_ringFinger1 = 20,
    Left_ringFinger2 = 21,
    Left_ringFinger3 = 22,
    Left_littleFinger1 = 23,
    Left_littleFinger2 = 24,
    Left_littleFinger3 = 25,
    Right_shoulder = 26,
    Right_arm = 27,
    Right_elbow = 28,
    Right_wrist = 29,
    Right_middleFinger1 = 30,
    Right_middleFinger2 = 31,
    Right_middleFinger3 = 32,
    Right_thumb0 = 33,
    Right_thumb1 = 34,
    Right_thumb2 = 35,
    Right_indexFinger1 = 36,
    Right_indexFinger2 = 37,
    Right_indexFinger3 = 38,
    Right_ringFinger1 = 39,
    Right_ringFinger2 = 40,
    Right_ringFinger3 = 41,
    Right_littleFinger1 = 42,
    Right_littleFinger2 = 43,
    Right_littleFinger3 = 44,
    Right_leg = 45,
    Right_knee = 46,
    Right_ankle = 47,
    Right_toe = 48,
    Left_leg = 49,
    Left_knee = 50,
    Left_ankle = 51,
    Left_toe = 52
  };

  struct Interpolant {
    unsigned int numParameterPositions;
    float *parameterPositions;
    float *resultBuffer;
    unsigned int numSampleValues;
    float *sampleValues;
    unsigned int valueSize;
  };
  struct Animation {
    float duration;
    std::vector<Interpolant> interpolants; // todo: pure array?
    unsigned int currentInterpolantIndex = 0;
    unsigned int index;
    std::string name;
  };
  struct AnimationMapping { // spec
    float *dst =  (float *)malloc((4) * sizeof(float));
    bool isPosition;
    unsigned int index;
    bool isFirstBone;
    bool isLastBone;
    bool isTop;
    bool isArm;
  };

  class Avatar {
  public:
    AnimationMixer *mixer;

    std::vector<std::string> strings;

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

    bool landWithMoving;
    bool flyState;
    bool doubleJumpState;
    bool jumpState;
    bool sitState;
    bool narutoRunState;
    bool holdState;
    bool pickUpState;

    std::string activateAnimationName;
    std::string fallLoopFrom;
    std::string sitAnimationName;
    std::string danceAnimationName;
    std::string emoteAnimationName;
    std::string useAnimationName;
    std::string useAnimationComboName;
    std::string hurtAnimationName;
    std::string unuseAnimationName;
    std::string aimAnimationName;
    std::string holdAnimationName;

    std::vector<std::string> useAnimationEnvelopeNames;
    
    void update(float *scratchStack);
  };
  class AnimationMixer {
  public:
    // static float now;
    static float nowS;

    Avatar *avatar;
    float *animationValues[53]; // 53 bones interpolants result buffers.

    float **update(float now, float nowS);
  };

  // ------
  // need run in this order
  void createAnimationMapping(bool isPosition, unsigned int index, bool isFirstBone, bool isLastBone, bool isTop, bool isArm);
  Animation *createAnimation(char *scratchStack, unsigned int nameByteLength, float duration);
  void createAnimationInterpolant(Animation *animation, unsigned int numParameterPositions, float *parameterPositions, unsigned int numSampleValues, float *sampleValues, unsigned int valueSize);
  void setAnimationGroup(Animation *animation, char *scratchStack, unsigned int groupNameByteLength, unsigned int keyNameByteLength, unsigned int keyNameUInt);
  void initAnimationSystem(float *scratchStack);
  AnimationMixer *createAnimationMixer();
  Avatar *createAnimationAvatar(AnimationMixer *mixer);
  // end: need run in this order
};

#endif // _ANIMATIONSYSTEM_H
