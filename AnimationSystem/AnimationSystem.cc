#include "AnimationSystem.h"
#include "CubicBezierEasing.h"

namespace AnimationSystem {
  std::vector<Avatar *> avatars;
  std::vector<AnimationMixer *> _animationMixers;
  std::vector<AnimationMapping> _animationMappings;
  std::map<std::string, Animation *> animationAll;
  std::vector<float> activateSpeedFactors;

  std::vector<std::vector<Animation *>> animationGroups;
  //
  std::vector<Animation *> singleAnimations;
  //
  std::vector<Animation *> walkAnimations;
  std::vector<Animation *> runAnimations;
  std::vector<Animation *> crouchAnimations;
  //
  std::vector<Animation *> activateAnimations;
  std::vector<Animation *> aimAnimations;
  std::vector<Animation *> danceAnimations;
  std::vector<Animation *> emoteAnimations;
  std::vector<Animation *> holdAnimations;
  std::vector<Animation *> hurtAnimations;
  std::vector<Animation *> landAnimations;
  std::vector<Animation *> narutoRunAnimations;
  std::vector<Animation *> pickUpAnimations;
  std::vector<Animation *> sitAnimations;
  std::vector<Animation *> swimAnimations;
  std::vector<Animation *> useAnimations;

  // todo: real default index instead of 0.
  unsigned int defaultSitAnimationIndex = 0;
  unsigned int defaultEmoteAnimationIndex = 0;
  unsigned int defaultDanceAnimationIndex = 0;
  unsigned int defaultHoldAnimationIndex = 0;
  unsigned int defaultActivateAnimationIndex = 0;
  unsigned int defaultNarutoRunAnimationIndex = 0;

  float localVectorArr[3];
  float localQuaternionArr[4];
  float localVecQuatArr[4];

  float *localVecQuatPtr;
  float *localVecQuatPtr2;

  float localWeights[6];

  float identityQuaternion[4] = {0, 0, 0, 1};

  bool isInitedAnimationSystem = false;

  // functions:

  // Utils ------

  float min(float a, float b) {
    if (a > b) {
      return b;
    } else {
      return a;
    }
  }
  float max(float a, float b) {
    if (a > b) {
      return a;
    } else {
      return b;
    }
  }
  float clamp(float value, float minValue, float maxValue) {
    return max( minValue, min( maxValue, value ) );
  }
  void _clearXZ(float *dst, bool isPosition) {
    if (isPosition) {
      dst[0] = 0;
      dst[2] = 0;
    }
  }
  void copyValue(float *dst, float *src, bool isPosition) {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    if (!isPosition) dst[3] = src[3];
  }
  void lerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t) {
    float x0 = src0[srcOffset0 + 0];
    float y0 = src0[srcOffset0 + 1];
    float z0 = src0[srcOffset0 + 2];

    float x1 = src1[srcOffset1 + 0];
    float y1 = src1[srcOffset1 + 1];
    float z1 = src1[srcOffset1 + 2];

    dst[dstOffset + 0] = x0 + (x1 - x0) * t;
    dst[dstOffset + 1] = y0 + (y1 - y0) * t;
    dst[dstOffset + 2] = z0 + (z1 - z0) * t;
  };
  void slerpFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t) {

    // fuzz-free, array-based Quaternion SLERP operation

    float x0 = src0[srcOffset0 + 0],
          y0 = src0[srcOffset0 + 1],
          z0 = src0[srcOffset0 + 2],
          w0 = src0[srcOffset0 + 3];

    float x1 = src1[srcOffset1 + 0],
          y1 = src1[srcOffset1 + 1],
          z1 = src1[srcOffset1 + 2],
          w1 = src1[srcOffset1 + 3];

    if (t == 0) {

      dst[dstOffset + 0] = x0;
      dst[dstOffset + 1] = y0;
      dst[dstOffset + 2] = z0;
      dst[dstOffset + 3] = w0;
      return;
    }

    if (t == 1) {

      dst[dstOffset + 0] = x1;
      dst[dstOffset + 1] = y1;
      dst[dstOffset + 2] = z1;
      dst[dstOffset + 3] = w1;
      return;
    }

    if (w0 != w1 || x0 != x1 || y0 != y1 || z0 != z1) {

      float s = 1 - t;
      float cos = x0 * x1 + y0 * y1 + z0 * z1 + w0 * w1,
            dir = (cos >= 0 ? 1 : -1),
            sqrSin = 1 - cos * cos;

      // Skip the Slerp for tiny steps to avoid numeric problems:
      float EPSILON = 2.220446049250313e-16;
      if (sqrSin > EPSILON) {

        float sinVal = sqrt(sqrSin),
              len = atan2(sinVal, cos * dir);

        s = sin(s * len) / sinVal;
        t = sin(t * len) / sinVal;
      }

      float tDir = t * dir;

      x0 = x0 * s + x1 * tDir;
      y0 = y0 * s + y1 * tDir;
      z0 = z0 * s + z1 * tDir;
      w0 = w0 * s + w1 * tDir;

      // Normalize in case we just did a lerp:
      if (s == 1 - t) {

        float f = 1 / sqrt(x0 * x0 + y0 * y0 + z0 * z0 + w0 * w0);

        x0 *= f;
        y0 *= f;
        z0 *= f;
        w0 *= f;
      }
    }

    dst[dstOffset] = x0;
    dst[dstOffset + 1] = y0;
    dst[dstOffset + 2] = z0;
    dst[dstOffset + 3] = w0;
  }
  void interpolateFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1, float t, bool isPosition) {
    if (isPosition) {
      lerpFlat(dst, dstOffset, src0, srcOffset0, src1, srcOffset1, t);
    } else {
      slerpFlat(dst, dstOffset, src0, srcOffset0, src1, srcOffset1, t);
    }
  }
  void multiplyQuaternionsFlat(float *dst, unsigned int dstOffset, float *src0, unsigned int srcOffset0, float *src1, unsigned int srcOffset1) {
    float x0 = src0[ srcOffset0 ];
    float y0 = src0[ srcOffset0 + 1 ];
    float z0 = src0[ srcOffset0 + 2 ];
    float w0 = src0[ srcOffset0 + 3 ];

    float x1 = src1[ srcOffset1 ];
    float y1 = src1[ srcOffset1 + 1 ];
    float z1 = src1[ srcOffset1 + 2 ];
    float w1 = src1[ srcOffset1 + 3 ];

    dst[ dstOffset ] = x0 * w1 + w0 * x1 + y0 * z1 - z0 * y1;
    dst[ dstOffset + 1 ] = y0 * w1 + w0 * y1 + z0 * x1 - x0 * z1;
    dst[ dstOffset + 2 ] = z0 * w1 + w0 * z1 + x0 * y1 - y0 * x1;
    dst[ dstOffset + 3 ] = w0 * w1 - x0 * x1 - y0 * y1 - z0 * z1;
  }
  void invertQuaternionFlat(float *dst, unsigned int dstOffset) {
    dst[ dstOffset ] *= -1;
    dst[ dstOffset + 1 ] *= -1;
    dst[ dstOffset + 2 ] *= -1;
  }
  void addVectorsFlat(float *dst, float *src0, float *src1) {
    dst[0] = src0[0] + src1[0];
    dst[1] = src0[1] + src1[1];
    dst[2] = src0[2] + src1[2];
  }
  void subVectorsFlat(float *dst, float *src0, float *src1) {
    dst[0] = src0[0] - src1[0];
    dst[1] = src0[1] - src1[1];
    dst[2] = src0[2] - src1[2];
  }
  
  // Main ------

  Avatar *createAnimationAvatar(AnimationMixer *mixer) {
    // std::cout << "createAnimationAvatar ------------------" << std::endl;

    // test
    // for (auto const& x : animationGroups) {
    //   std::map<std::string, Animation *> animationGroup = animationGroups[x.first];
    //   for (auto const& y : animationGroup) {
    //     Animation *animation = animationGroup[y.first];
    //     std::cout << " groupName: " << x.first << " keyName: " << y.first << " name: " << animation->name << std::endl;
    //   }
    // }

    // avatar->mixer = createAnimationMixer();
    Avatar *avatar = new Avatar();
    avatars.push_back(avatar);
    avatar->mixer = mixer;
    mixer->avatar = avatar;

    return avatar;
  }
  void initAnimationSystem(float *scratchStack) { // only need init once globally
    if (!isInitedAnimationSystem) {
      std::cout << "initAnimationSystem ------------------" << std::endl;

      // -------------------------------------------------------------------------

      // note: Need be the same order with `constants.h/enum SingleAnimationIndex`.
      singleAnimations.push_back(animationAll["idle.fbx"]); // Idle
      singleAnimations.push_back(animationAll["Crouch Idle.fbx"]); // CrouchIdle
      singleAnimations.push_back(animationAll["jump.fbx"]); // Jump
      singleAnimations.push_back(animationAll["jump_double.fbx"]); // DoubleJump
      singleAnimations.push_back(animationAll["falling.fbx"]); // FallLoop
      singleAnimations.push_back(animationAll["treading water.fbx"]); // Float
      animationGroups.push_back(singleAnimations);

      // ---

      // 8 directions walk animations ---
      // note: Need be the same order with `constants.h/enum WalkAnimationIndex`.
      walkAnimations.push_back(animationAll["walking.fbx"]); // Forward
      walkAnimations.push_back(animationAll["walking backwards.fbx"]); // Backward
      walkAnimations.push_back(animationAll["left strafe walking.fbx"]); // Left
      walkAnimations.push_back(animationAll["right strafe walking reverse.fbx"]); // LeftMirror
      walkAnimations.push_back(animationAll["right strafe walking.fbx"]); // Right
      walkAnimations.push_back(animationAll["left strafe walking reverse.fbx"]); // RightMirror
      animationGroups.push_back(walkAnimations);

      // 8 directions run animations ---
      // note: Need be the same order with `constants.h/enum RunAnimationIndex`.
      runAnimations.push_back(animationAll["Fast Run.fbx"]); // Forward
      runAnimations.push_back(animationAll["running backwards.fbx"]); // Backward
      runAnimations.push_back(animationAll["left strafe.fbx"]); // Left
      runAnimations.push_back(animationAll["right strafe reverse.fbx"]); // LeftMirror
      runAnimations.push_back(animationAll["right strafe.fbx"]); // Right
      runAnimations.push_back(animationAll["left strafe reverse.fbx"]); // RightMirror
      animationGroups.push_back(runAnimations);

      // 8 directions crouch animations ---
      // note: Need be the same order with `constants.h/enum CrouchAnimationIndex`.
      crouchAnimations.push_back(animationAll["Sneaking Forward.fbx"]); // Forward
      crouchAnimations.push_back(animationAll["Sneaking Forward reverse.fbx"]); // Backward
      crouchAnimations.push_back(animationAll["Crouched Sneaking Left.fbx"]); // Left
      crouchAnimations.push_back(animationAll["Crouched Sneaking Right reverse.fbx"]); // LeftMirror
      crouchAnimations.push_back(animationAll["Crouched Sneaking Right.fbx"]); // Right
      crouchAnimations.push_back(animationAll["Crouched Sneaking Left reverse.fbx"]); // RightMirror
      animationGroups.push_back(crouchAnimations);

      // -------------------------------------------------------------------------

      // note: Need be the same order with `constants.h/enum ActivateAnimationIndex`.
      activateAnimations.push_back(animationAll["grab_forward.fbx"]); // Grab_forward
      activateAnimations.push_back(animationAll["grab_down.fbx"]); // Grab_down
      activateAnimations.push_back(animationAll["grab_up.fbx"]); // Grab_up
      activateAnimations.push_back(animationAll["grab_left.fbx"]); // Grab_left
      activateAnimations.push_back(animationAll["grab_right.fbx"]); // Grab_right
      activateAnimations.push_back(animationAll["pick_up.fbx"]); // Pick_up
      animationGroups.push_back(activateAnimations);

      unsigned int index = 0;
      activateSpeedFactors.push_back(scratchStack[index++]); // Grab_forward
      activateSpeedFactors.push_back(scratchStack[index++]); // Grab_down
      activateSpeedFactors.push_back(scratchStack[index++]); // Grab_up
      activateSpeedFactors.push_back(scratchStack[index++]); // Grab_left
      activateSpeedFactors.push_back(scratchStack[index++]); // Grab_right
      activateSpeedFactors.push_back(scratchStack[index++]); // Pick_up

      // note: Need be the same order with `constants.h/enum AimAnimationIndex`.
      aimAnimations.push_back(animationAll["sword_idle_side.fbx"]); // SwordSideIdle
      aimAnimations.push_back(animationAll["sword_side_slash.fbx"]); // SwordSideSlash
      aimAnimations.push_back(animationAll["sword_side_slash_step.fbx"]); // SwordSideSlashStep
      aimAnimations.push_back(animationAll["sword_topdown_slash.fbx"]); // SwordTopDownSlash
      aimAnimations.push_back(animationAll["sword_topdown_slash_step.fbx"]); // SwordTopDownSlashStep
      animationGroups.push_back(aimAnimations);

      // note: Need be the same order with `constants.h/enum DanceAnimationIndex`.
      danceAnimations.push_back(animationAll["Hip Hop Dancing.fbx"]); // Dansu
      danceAnimations.push_back(animationAll["powerup.fbx"]); // Powerup
      animationGroups.push_back(danceAnimations);

      // note: Need be the same order with `constants.h/enum EmoteAnimationIndex`.
      emoteAnimations.push_back(animationAll["alert.fbx"]); // Alert
      emoteAnimations.push_back(animationAll["alert_soft.fbx"]); // AlertSoft
      emoteAnimations.push_back(animationAll["angry.fbx"]); // Angry
      emoteAnimations.push_back(animationAll["angry_soft.fbx"]); // AngrySoft
      emoteAnimations.push_back(animationAll["embarrassed.fbx"]); // Embarrassed
      emoteAnimations.push_back(animationAll["embarrassed_soft.fbx"]); // EmbarrassedSoft
      emoteAnimations.push_back(animationAll["head_nod.fbx"]); // HeadNod
      emoteAnimations.push_back(animationAll["head_nod_single.fbx"]); // HeadNodSoft
      emoteAnimations.push_back(animationAll["head_shake.fbx"]); // HeadShake
      emoteAnimations.push_back(animationAll["head_shake_single.fbx"]); // HeadShakeSoft
      emoteAnimations.push_back(animationAll["sad.fbx"]); // Sad
      emoteAnimations.push_back(animationAll["sad_soft.fbx"]); // SadSoft
      emoteAnimations.push_back(animationAll["surprise.fbx"]); // Surprise
      emoteAnimations.push_back(animationAll["surprise_soft.fbx"]); // SurpriseSoft
      emoteAnimations.push_back(animationAll["victory.fbx"]); // Victory
      emoteAnimations.push_back(animationAll["victory_soft.fbx"]); // VictorySoft
      animationGroups.push_back(emoteAnimations);

      // // note: Need be the same order with `constants.h/enum HoldAnimationIndex`.
      holdAnimations.push_back(animationAll["pick_up_idle.fbx"]); // Pick_up_idle
      animationGroups.push_back(holdAnimations);

      // note: Need be the same order with `constants.h/enum HurtAnimationIndex`.
      hurtAnimations.push_back(animationAll["pain_back.fbx"]); // Pain_back
      hurtAnimations.push_back(animationAll["pain_arch.fbx"]); // Pain_arch
      animationGroups.push_back(hurtAnimations);

      // note: Need be the same order with `constants.h/enum LandAnimationIndex`.
      landAnimations.push_back(animationAll["landing.fbx"]); // Landing
      landAnimations.push_back(animationAll["landing 2.fbx"]); // Landing2
      animationGroups.push_back(landAnimations);

      // note: Need be the same order with `constants.h/enum NarutoRunAnimationIndex`.
      narutoRunAnimations.push_back(animationAll["naruto run.fbx"]); // NarutoRun
      animationGroups.push_back(narutoRunAnimations);

      // note: Need be the same order with `constants.h/enum PickUpAnimationIndex`.
      pickUpAnimations.push_back(animationAll["pick_up.fbx"]); // PickUp
      pickUpAnimations.push_back(animationAll["pick_up_idle.fbx"]); // PickUpIdle
      pickUpAnimations.push_back(animationAll["pick_up_throw.fbx"]); // PickUpThrow
      pickUpAnimations.push_back(animationAll["put_down.fbx"]); // PutDown
      pickUpAnimations.push_back(animationAll["pick_up_zelda.fbx"]); // PickUpZelda
      pickUpAnimations.push_back(animationAll["pick_up_idle_zelda.fbx"]); // PickUpIdleZelda
      pickUpAnimations.push_back(animationAll["put_down_zelda.fbx"]); // PutDownZelda
      animationGroups.push_back(pickUpAnimations);

      // note: Need be the same order with `constants.h/enum SitAnimationIndex`.
      sitAnimations.push_back(animationAll["sitting idle.fbx"]); // Chair
      sitAnimations.push_back(animationAll["sitting idle.fbx"]); // Saddle
      sitAnimations.push_back(animationAll["Skateboarding.fbx"]); // Stand
      animationGroups.push_back(sitAnimations);

      // note: Need be the same order with `constants.h/enum SwimAnimationIndex`.
      swimAnimations.push_back(animationAll["Swimming.fbx"]); // Breaststroke
      swimAnimations.push_back(animationAll["freestyle.fbx"]); // Freestyle
      animationGroups.push_back(swimAnimations);

      // note: Need be the same order with `constants.h/enum UseAnimationIndex`.
      useAnimations.push_back(animationAll["One Hand Sword Combo.fbx"]); // Combo
      useAnimations.push_back(animationAll["sword and shield slash.fbx"]); // Slash
      useAnimations.push_back(animationAll["Rifle Aiming Idle.fbx"]); // Rifle
      useAnimations.push_back(animationAll["Pistol Aiming Idle.fbx"]); // Pistol
      useAnimations.push_back(animationAll["magic cast.fbx"]); // Magic
      useAnimations.push_back(animationAll["eating.fbx"]); // Eat
      useAnimations.push_back(animationAll["drinking.fbx"]); // Drink
      useAnimations.push_back(animationAll["pick_up_throw.fbx"]); // Throw
      useAnimations.push_back(animationAll["pick_up_throw.fbx"]); // PickUpThrow
      useAnimations.push_back(animationAll["bow draw.fbx"]); // BowDraw
      useAnimations.push_back(animationAll["bow idle.fbx"]); // BowIdle
      useAnimations.push_back(animationAll["bow loose.fbx"]); // BowLoose
      useAnimations.push_back(animationAll["pickaxe_swing.fbx"]); // Pickaxe
      useAnimations.push_back(animationAll["sword_idle_side.fbx"]); // SwordSideIdle
      useAnimations.push_back(animationAll["sword_side_slash.fbx"]); // SwordSideSlash
      useAnimations.push_back(animationAll["sword_side_slash_step.fbx"]); // SwordSideSlashStep
      useAnimations.push_back(animationAll["sword_topdown_slash.fbx"]); // SwordTopDownSlash
      useAnimations.push_back(animationAll["sword_topdown_slash_step.fbx"]); // SwordTopDownSlashSte
      animationGroups.push_back(useAnimations);

      // -------------------------------------------------------------------------

      std::cout << "test animation name: " << useAnimations[(int)UseAnimationIndex::Drink]->name << std::endl;
      std::cout << "test animation name: " << useAnimations[(int)UseAnimationIndex::SwordSideSlashStep]->name << std::endl;
      std::cout << "test animation name: " << useAnimations[(int)UseAnimationIndex::Throw]->name << std::endl;

      // -------------------------------------------------------------------------

      CubicBezierEasing::init(0, 1, 0, 1);

      // -------------------------------------------------------------------------

      // for (float i = 0; i <= 1; i+=0.1) {
      //   std::cout << "cubicBezier " << i << ": " << CubicBezierEasing::cubicBezier(i);
      // }
      float i, result;
      i = 0;
      result = CubicBezierEasing::cubicBezier(i);
      std::cout << "cubicBezier " << i << ": " << result << std::endl;
      i = 0.1;
      result = CubicBezierEasing::cubicBezier(i);
      std::cout << "cubicBezier " << i << ": " << result << std::endl;
      i = 0.2;
      result = CubicBezierEasing::cubicBezier(i);
      std::cout << "cubicBezier " << i << ": " << result << std::endl;
      i = 0.3;
      result = CubicBezierEasing::cubicBezier(i);
      std::cout << "cubicBezier " << i << ": " << result << std::endl;
      i = 0.4;
      result = CubicBezierEasing::cubicBezier(i);
      std::cout << "cubicBezier " << i << ": " << result << std::endl;
      i = 0.5;
      result = CubicBezierEasing::cubicBezier(i);
      std::cout << "cubicBezier " << i << ": " << result << std::endl;
      i = 0.6;
      result = CubicBezierEasing::cubicBezier(i);
      std::cout << "cubicBezier " << i << ": " << result << std::endl;
      i = 0.7;
      result = CubicBezierEasing::cubicBezier(i);
      std::cout << "cubicBezier " << i << ": " << result << std::endl;
      i = 0.8;
      result = CubicBezierEasing::cubicBezier(i);
      std::cout << "cubicBezier " << i << ": " << result << std::endl;
      i = 0.9;
      result = CubicBezierEasing::cubicBezier(i);
      std::cout << "cubicBezier " << i << ": " << result << std::endl;
      i = 1;
      result = CubicBezierEasing::cubicBezier(i);
      std::cout << "cubicBezier " << i << ": " << result << std::endl;

      //
      isInitedAnimationSystem = true;
    }
  }
  void Avatar::update(float *scratchStack) {
    unsigned int index = 0;

    // values ---
    this->forwardFactor = scratchStack[index++];
    this->backwardFactor = scratchStack[index++];
    this->leftFactor = scratchStack[index++];
    this->rightFactor = scratchStack[index++];
    this->mirrorLeftFactorReverse = scratchStack[index++];
    this->mirrorLeftFactor = scratchStack[index++];
    this->mirrorRightFactorReverse = scratchStack[index++];
    this->mirrorRightFactor = scratchStack[index++];

    this->idleWalkFactor = scratchStack[index++];
    this->walkRunFactor = scratchStack[index++];
    this->crouchFactor = scratchStack[index++];
    this->sprintFactor = scratchStack[index++];
    this->movementsTransitionFactor = scratchStack[index++];

    this->activateTime = scratchStack[index++];
    this->swimTime = scratchStack[index++];
    this->movementsTime = scratchStack[index++];

    // action states ---
    this->jumpState = scratchStack[index++];
    this->doubleJumpState = scratchStack[index++];
    this->flyState = scratchStack[index++];
    this->narutoRunState = scratchStack[index++];
    this->sitState = scratchStack[index++];
    this->holdState = scratchStack[index++];
    this->pickUpState = scratchStack[index++];
    this->swimState = scratchStack[index++];

    // other ---
    this->landWithMoving = scratchStack[index++];
    this->landTime = scratchStack[index++];
    this->fallLoopFactor = scratchStack[index++];
    this->fallLoopTime = scratchStack[index++];
    this->flyTime = scratchStack[index++];
    this->doubleJumpTime = scratchStack[index++];
    this->jumpTime = scratchStack[index++];
    this->narutoRunTime = scratchStack[index++];
    this->narutoRunTimeFactor = scratchStack[index++];
    this->danceFactor = scratchStack[index++];
    this->crouchMaxTime = scratchStack[index++];
    this->emoteFactor = scratchStack[index++];
    this->lastEmoteTime = scratchStack[index++];
    this->useTime = scratchStack[index++];
    this->useAnimationEnvelopeLength = scratchStack[index++];
    this->hurtTime = scratchStack[index++];
    this->unuseTime = scratchStack[index++];
    this->aimTime = scratchStack[index++];
    this->aimMaxTime = scratchStack[index++];
    this->pickUpTime = scratchStack[index++];

    this->useAnimationIndex = (int)(scratchStack[index++]);
    this->emoteAnimationIndex = (int)(scratchStack[index++]);
    this->sitAnimationIndex = (int)(scratchStack[index++]);
    this->danceAnimationIndex = (int)(scratchStack[index++]);
    this->activateAnimationIndex = (int)(scratchStack[index++]);
    this->hurtAnimationIndex = (int)(scratchStack[index++]);
    this->useAnimationComboIndex = (int)(scratchStack[index++]); // todo: use real combo index.
    this->unuseAnimationIndex = (int)(scratchStack[index++]);
    this->aimAnimationIndex = (int)(scratchStack[index++]);

    this->fallLoopFrom = (int)(scratchStack[index++]);
    this->landTimeS = scratchStack[index++];
    this->timeSinceLastMoveS = scratchStack[index++];

    this->useAnimationEnvelopeIndices.clear();
    for (unsigned int i = 0; i < useAnimationEnvelopeLength; i++) {
      this->useAnimationEnvelopeIndices.push_back((int)(scratchStack[index++]));
    }

    //
    
    localWeights[0] = this->forwardFactor;
    localWeights[1] = this->backwardFactor;
    localWeights[2] = this->mirrorLeftFactorReverse;
    localWeights[3] = this->mirrorLeftFactor;
    localWeights[4] = this->mirrorRightFactorReverse;
    localWeights[5] = this->mirrorRightFactor;
  }
  AnimationMixer *createAnimationMixer() {
    AnimationMixer *animationMixer = new AnimationMixer();
    _animationMixers.push_back(animationMixer);
    animationMixer->animationValues = new float *[_animationMappings.size()];
    return animationMixer;
  }
  void createAnimationMapping(bool isPosition, unsigned int index, bool isFirstBone, bool isLastBone, bool isTop, bool isArm) {
    AnimationMapping animationMapping;
    animationMapping.isPosition = isPosition;
    animationMapping.index = index;
    animationMapping.isFirstBone = isFirstBone;
    animationMapping.isLastBone = isLastBone;
    animationMapping.isTop = isTop;
    animationMapping.isArm = isArm;
    _animationMappings.push_back(animationMapping);
  }
  Animation *createAnimation(char *scratchStack, unsigned int nameByteLength, float duration) {
    Animation *animation = new Animation();
    animation->duration = duration;
    std::string name = "";
    for (unsigned int i = 0; i < nameByteLength; i++) {
      name += scratchStack[i];
    }
    animation->name = name;

    animationAll[name] = animation;

    return animation;
  }
  void createAnimationInterpolant(Animation *animation, unsigned int numParameterPositions, float *parameterPositions, unsigned int numSampleValues, float *sampleValues, unsigned int valueSize) {
    Interpolant interpolant;
    interpolant.numParameterPositions = numParameterPositions;
    interpolant.parameterPositions = parameterPositions;
    interpolant.resultBuffer = (float *)malloc(valueSize * sizeof(float));
    interpolant.numSampleValues = numSampleValues;
    interpolant.sampleValues = sampleValues;
    interpolant.valueSize = valueSize; // only support 3 (vector) or 4 (quaternion)

    animation->interpolants.push_back(interpolant);
  }
  float *evaluateInterpolant(Animation *animation, unsigned int interpolantIndex, float t) { // todo: move to Utils.
    Interpolant interpolant = animation->interpolants[interpolantIndex];

    if (interpolant.numParameterPositions == 1) {
      interpolant.resultBuffer[0] = interpolant.sampleValues[0];
      interpolant.resultBuffer[1] = interpolant.sampleValues[1];
      interpolant.resultBuffer[2] = interpolant.sampleValues[2];
      if (interpolant.valueSize == 4) {
        interpolant.resultBuffer[3] = interpolant.sampleValues[3];
      }
    } else {
      int index = 0;
      for (; index < interpolant.numParameterPositions; index++) {
        if (interpolant.parameterPositions[index] > t) {
          break;
        }
      }

      if (index == 0) { // note: Handle situation that, parameterPositions[0] > 0, and t == 0 or t < parameterPositions[0].
        interpolant.resultBuffer[0] = interpolant.sampleValues[0];
        interpolant.resultBuffer[1] = interpolant.sampleValues[1];
        interpolant.resultBuffer[2] = interpolant.sampleValues[2];
        if (interpolant.valueSize == 4) {
          interpolant.resultBuffer[3] = interpolant.sampleValues[3];
        }
      } else if (index > interpolant.numParameterPositions - 1) { // note: Handle situation that, t > max parameterPosition.
        unsigned int maxIndex = interpolant.numParameterPositions - 1;
        interpolant.resultBuffer[0] = interpolant.sampleValues[maxIndex * interpolant.valueSize + 0];
        interpolant.resultBuffer[1] = interpolant.sampleValues[maxIndex * interpolant.valueSize + 1];
        interpolant.resultBuffer[2] = interpolant.sampleValues[maxIndex * interpolant.valueSize + 2];
        if (interpolant.valueSize == 4) {
          interpolant.resultBuffer[3] = interpolant.sampleValues[maxIndex * interpolant.valueSize + 3];
        }
      } else {
        unsigned int index0 = index - 1;
        unsigned int index1 = index;

        float time0 = interpolant.parameterPositions[index0];
        float time1 = interpolant.parameterPositions[index1];
        float f = (t - time0) / (time1 - time0);

        interpolateFlat(
          interpolant.resultBuffer, 0,
          interpolant.sampleValues, index0 * interpolant.valueSize,
          interpolant.sampleValues, index1 * interpolant.valueSize,
          f,
          interpolant.valueSize == 3
        );
      }
    }

    return interpolant.resultBuffer;
  }

  float *doBlendList(AnimationMapping &spec, std::vector<Animation *> &animations, float &timeS) { // note: Big performance influnce!!! Use `&` to prevent copy parameter's values!!!
    float *resultVecQuat;
    unsigned int indexWeightBigThanZero = 0;
    float currentWeight = 0;
    for (int i = 0; i < animations.size(); i++) {
    // for (auto const& x : animations) {
      // if (spec.isPosition) std::cout << " x.first: " << x.first << std::endl;
      // float weight = localWeights[x.first];
      float weight = localWeights[i];
      if (weight > 0) {
        // Animation *animation = animations[x.first];
        Animation *animation = animations[i];
        float *vecQuat = evaluateInterpolant(animation, spec.index, fmod(timeS, animation->duration));
        if (indexWeightBigThanZero == 0) {
          resultVecQuat = vecQuat;

          indexWeightBigThanZero++;
          currentWeight = weight;
        } else {
          float t = weight / (currentWeight + weight);
          interpolateFlat(resultVecQuat, 0, resultVecQuat, 0, vecQuat, 0, t, spec.isPosition);

          indexWeightBigThanZero++;
          currentWeight += weight;
        }
      }
    }
    return resultVecQuat;
  }

  void _handleDefault(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_handleDefault, "; // test: blend strings.

    // note: Big performance influnce!!! Do not update `localWeights` here, because of will run 53 times ( 53 bones )!!! todo: Notice codes which will run 53 times!!!
    // localWeights["forward"] = avatar->forwardFactor;
    // localWeights["backward"] = avatar->backwardFactor;
    // localWeights["left"] = avatar->mirrorLeftFactorReverse;
    // localWeights["leftMirror"] = avatar->mirrorLeftFactor;
    // localWeights["right"] = avatar->mirrorRightFactorReverse;
    // localWeights["rightMirror"] = avatar->mirrorRightFactor;

    // walkAnimations
    localVecQuatPtr2 = doBlendList(spec, walkAnimations, avatar->landTimeS);
    copyValue(spec.dst, localVecQuatPtr2, spec.isPosition);

    // if (avatar->walkRunFactor > 0) {
      // runAnimations
      localVecQuatPtr2 = doBlendList(spec, runAnimations, avatar->landTimeS);

      // blend walk run
      interpolateFlat(spec.dst, 0, spec.dst, 0, localVecQuatPtr2, 0, avatar->walkRunFactor, spec.isPosition);
      _clearXZ(spec.dst, spec.isPosition);
    // }

    // blend idle ---
    // if (avatar->idleWalkFactor < 1) {
      localVecQuatPtr = evaluateInterpolant(singleAnimations[(int)SingleAnimationIndex::Idle], spec.index, fmod(avatar->timeSinceLastMoveS, singleAnimations[(int)SingleAnimationIndex::Idle]->duration));
      interpolateFlat(spec.dst, 0, spec.dst, 0, localVecQuatPtr, 0, 1 - avatar->idleWalkFactor, spec.isPosition);
    // }

    // if (avatar->crouchFactor > 0) {
      // crouchAnimations
      localVecQuatPtr2 = doBlendList(spec, crouchAnimations, avatar->landTimeS);
      copyValue(localVecQuatArr, localVecQuatPtr2, spec.isPosition);
      _clearXZ(localVecQuatArr, spec.isPosition);

      // blend crouch idle ---
      // if (avatar->idleWalkFactor < 1) {
        localVecQuatPtr = evaluateInterpolant(singleAnimations[(int)SingleAnimationIndex::CrouchIdle], spec.index, fmod(avatar->timeSinceLastMoveS, singleAnimations[(int)SingleAnimationIndex::CrouchIdle]->duration));
        interpolateFlat(localVecQuatArr, 0, localVecQuatArr, 0, localVecQuatPtr, 0, 1 - avatar->idleWalkFactor, spec.isPosition);
      // }

      // blend walkRun and crouch
      interpolateFlat(spec.dst, 0, spec.dst, 0, localVecQuatArr, 0, avatar->crouchFactor, spec.isPosition);
    // }
  }

  void _blendDoubleJump(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendDoubleJump, "; // test: blend strings.

    float t2 = avatar->doubleJumpTime / 1000;
    float *v2 = evaluateInterpolant(singleAnimations[(int)SingleAnimationIndex::DoubleJump], spec.index, t2);

    copyValue(spec.dst, v2, spec.isPosition);

    _clearXZ(spec.dst, spec.isPosition);
  }

  void _blendJump(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendJump, "; // test: blend strings.

    float t2 = avatar->jumpTime / 1000;
    float *v2 = evaluateInterpolant(singleAnimations[(int)SingleAnimationIndex::Jump], spec.index, t2);

    copyValue(spec.dst, v2, spec.isPosition);

    _clearXZ(spec.dst, spec.isPosition);

    if (avatar->holdState && spec.isArm) {
      Animation *holdAnimation = holdAnimations[defaultHoldAnimationIndex];
      float t2 = fmod(AnimationMixer::nowS, holdAnimation->duration);
      float *v2 = evaluateInterpolant(holdAnimation, spec.index, t2);
      copyValue(spec.dst, v2, spec.isPosition);
    }
  }

  void _blendSit(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendSit, "; // test: blend strings.

    Animation *sitAnimation = sitAnimations[avatar->sitAnimationIndex == -1 ? defaultSitAnimationIndex : avatar->sitAnimationIndex];
    float *v2 = evaluateInterpolant(sitAnimation, spec.index, 1);

    copyValue(spec.dst, v2, spec.isPosition);
  }

  void _blendNarutoRun(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendNarutoRun, "; // test: blend strings.

    Animation *narutoRunAnimation = narutoRunAnimations[defaultNarutoRunAnimationIndex];
    float t2 = fmod((avatar->narutoRunTime / 1000 * avatar->narutoRunTimeFactor), narutoRunAnimation->duration);
    float *v2 = evaluateInterpolant(narutoRunAnimation, spec.index, t2);

    copyValue(spec.dst, v2, spec.isPosition);

    _clearXZ(spec.dst, spec.isPosition);
  }

  void _blendDance(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendDance, "; // test: blend strings.

    _handleDefault(spec, avatar);

    Animation *danceAnimation = danceAnimations[avatar->danceAnimationIndex < 0 ? defaultDanceAnimationIndex : avatar->danceAnimationIndex];
    float t2 = fmod(AnimationMixer::nowS, danceAnimation->duration);
    float *v2 = evaluateInterpolant(danceAnimation, spec.index, t2);

    float danceFactorS = avatar->danceFactor / avatar->crouchMaxTime;
    float f = min(max(danceFactorS, 0), 1);
    
    interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);

    _clearXZ(spec.dst, spec.isPosition);
  }

  void _blendEmote(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendEmote, "; // test: blend strings.

    _handleDefault(spec, avatar);

    Animation *emoteAnimation = emoteAnimations[avatar->emoteAnimationIndex < 0 ? defaultEmoteAnimationIndex : avatar->emoteAnimationIndex];
    float emoteTime = AnimationMixer::nowS * 1000 - avatar->lastEmoteTime; // todo: use now.
    float t2 = min(emoteTime / 1000, emoteAnimation->duration);
    float *v2 = evaluateInterpolant(emoteAnimation, spec.index, t2);

    float emoteFactorS = avatar->emoteFactor / avatar->crouchMaxTime;
    float f = min(max(emoteFactorS, 0), 1);

    if (spec.index == BoneIndex::Spine || spec.index == BoneIndex::Chest || spec.index == BoneIndex::UpperChest || spec.index == BoneIndex::Neck || spec.index == BoneIndex::Head) {
      if (!spec.isPosition) {
        multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
      } else {
        interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
      }
    } else {
      if (!spec.isTop) {
        f *= (1 - avatar->idleWalkFactor);
      }

      interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
    }

    _clearXZ(spec.dst, spec.isPosition);
  }

  void _blendUse(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendUse, "; // test: blend strings.

    Animation *useAnimation = nullptr;
    float t2;
    float useTimeS = avatar->useTime / 1000;
    if (avatar->useAnimationIndex >= 0) {
      useAnimation = useAnimations[avatar->useAnimationIndex];
      t2 = min(useTimeS, useAnimation->duration);
    } else if(avatar->useAnimationComboIndex >= 0) {
      useAnimation = useAnimations[avatar->useAnimationComboIndex];
      t2 = min(useTimeS, useAnimation->duration);
    } else if (avatar->useAnimationEnvelopeIndices.size() > 0) {
      float totalTime = 0;
      for (unsigned int i = 0; i < avatar->useAnimationEnvelopeIndices.size() - 1; i++) {
        int animationIndex = avatar->useAnimationEnvelopeIndices[i];
        Animation *animation = useAnimations[animationIndex];
        totalTime += animation->duration;
      }

      if (totalTime > 0) {
        float animationTimeBase = 0;
        for (unsigned int i = 0; i < avatar->useAnimationEnvelopeIndices.size() - 1; i++) {
          int animationIndex = avatar->useAnimationEnvelopeIndices[i];
          Animation *animation = useAnimations[animationIndex];
          if (useTimeS < (animationTimeBase + animation->duration)) {
            useAnimation = animation;
            break;
          }
          animationTimeBase += animation->duration;
        }
        if (useAnimation != nullptr) { // first iteration
          t2 = min(useTimeS - animationTimeBase, useAnimation->duration);
        } else { // loop
          int secondLastAnimationIndex = avatar->useAnimationEnvelopeIndices[avatar->useAnimationEnvelopeIndices.size() - 2];
          useAnimation = useAnimations[secondLastAnimationIndex];
          t2 = fmod((useTimeS - animationTimeBase), useAnimation->duration);
        }
      }
    }
    
    _handleDefault(spec, avatar);

    if (useAnimation) {
      if (!spec.isPosition) {
        float *v2 = evaluateInterpolant(useAnimation, spec.index, t2);

        Animation *idleAnimation = singleAnimations[(int)SingleAnimationIndex::Idle];
        float t3 = 0;
        float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);

        invertQuaternionFlat(v3, 0);
        multiplyQuaternionsFlat(spec.dst, 0, v3, 0, spec.dst, 0);
        multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
      } else {
        float *v2 = evaluateInterpolant(useAnimation, spec.index, t2);
        _clearXZ(v2, spec.isPosition);

        Animation *idleAnimation = singleAnimations[(int)SingleAnimationIndex::Idle];
        float t3 = 0;
        float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);

        subVectorsFlat(spec.dst, spec.dst, v3);
        addVectorsFlat(spec.dst, spec.dst, v2);
      }
    }
  }

  void _blendHurt(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendHurt, "; // test: blend strings.

    _handleDefault(spec, avatar);

    Animation *hurtAnimation = hurtAnimations[avatar->hurtAnimationIndex];
    float hurtTimeS = avatar->hurtTime / 1000;
    float t2 = min(hurtTimeS, hurtAnimation->duration);
    if (!spec.isPosition) {
      if (hurtAnimation) {
        float *v2 = evaluateInterpolant(hurtAnimation, spec.index, t2);

        Animation *idleAnimation = singleAnimations[(int)SingleAnimationIndex::Idle];
        float t3 = 0;
        float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);
        
        invertQuaternionFlat(v3, 0);
        multiplyQuaternionsFlat(spec.dst, 0, v3, 0, spec.dst, 0);
        multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
      }
    } else {
      float *v2 = evaluateInterpolant(hurtAnimation, spec.index, t2);

      Animation *idleAnimation = singleAnimations[(int)SingleAnimationIndex::Idle];
      float t3 = 0;
      float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);

      subVectorsFlat(spec.dst, spec.dst, v3);
      addVectorsFlat(spec.dst, spec.dst, v2);
    }
  }

  void _blendAim(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendAim, "; // test: blend strings.
    
    _handleDefault(spec, avatar);

    Animation *aimAnimation = aimAnimations[avatar->aimAnimationIndex];
    float t2 = fmod((avatar->aimTime / avatar->aimMaxTime), aimAnimation->duration);
    if (!spec.isPosition) {
      if (aimAnimation) {
        float *v2 = evaluateInterpolant(aimAnimation, spec.index, t2);

        Animation *idleAnimation = singleAnimations[(int)SingleAnimationIndex::Idle]; // todo: don't always idle.fbx ? Walk Run Crouch ?
        float t3 = 0;
        float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);

        invertQuaternionFlat(v3, 0);
        multiplyQuaternionsFlat(spec.dst, 0, v3, 0, spec.dst, 0);
        multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
      }
    } else {
      float *v2 = evaluateInterpolant(aimAnimation, spec.index, t2);

      Animation *idleAnimation = singleAnimations[(int)SingleAnimationIndex::Idle];
      float t3 = 0;
      float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);

      subVectorsFlat(spec.dst, spec.dst, v3);
      addVectorsFlat(spec.dst, spec.dst, v2);
    }
  }

  void _blendUnuse(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendUnuse, "; // test: blend strings.
    
    _handleDefault(spec, avatar);

    float unuseTimeS = avatar->unuseTime / 1000;
    Animation *unuseAnimation = useAnimations[avatar->unuseAnimationIndex];
    float t2 = min(unuseTimeS, unuseAnimation->duration);
    float f = min(max(unuseTimeS / unuseAnimation->duration, 0), 1);
    float f2 = std::pow(1 - f, 2);

    if (!spec.isPosition) {
      float *v2 = evaluateInterpolant(unuseAnimation, spec.index, t2);

      Animation *idleAnimation = singleAnimations[(int)SingleAnimationIndex::Idle];
      float t3 = 0;
      float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);
        
      copyValue(localQuaternionArr, spec.dst, spec.isPosition);
      invertQuaternionFlat(v3, 0);
      multiplyQuaternionsFlat(localQuaternionArr, 0, v3, 0, localQuaternionArr, 0);
      multiplyQuaternionsFlat(localQuaternionArr, 0, v2, 0, localQuaternionArr, 0);

      interpolateFlat(spec.dst, 0, spec.dst, 0, localQuaternionArr, 0, f2, spec.isPosition);
    } else {
      float *v2 = evaluateInterpolant(unuseAnimation, spec.index, t2);

      Animation *idleAnimation = singleAnimations[(int)SingleAnimationIndex::Idle];
      float t3 = 0;
      float *v3 = evaluateInterpolant(idleAnimation, spec.index, t3);
      
      copyValue(localVectorArr, spec.dst, spec.isPosition);
      subVectorsFlat(localVectorArr, localVectorArr, v3);
      addVectorsFlat(localVectorArr, localVectorArr, v2);

      interpolateFlat(spec.dst, 0, spec.dst, 0, localVectorArr, 0, f2, spec.isPosition);
    }
  }

  void _blendHold(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendHold, "; // test: blend strings.
    
    _handleDefault(spec, avatar);

    Animation *holdAnimation = holdAnimations[defaultHoldAnimationIndex];
    float t2 = fmod(AnimationMixer::nowS, holdAnimation->duration);
    float *v2 = evaluateInterpolant(holdAnimation, spec.index, t2);

    if (spec.isTop) {
      if (spec.index == BoneIndex::Left_arm || spec.index == BoneIndex::Right_arm) {
        copyValue(spec.dst, v2, spec.isPosition);
      } else {
        if (spec.isArm) {
          float f = avatar->walkRunFactor * 0.7 + avatar->crouchFactor * (1 - avatar->idleWalkFactor) * 0.5;
          interpolateFlat(spec.dst, 0, spec.dst, 0, identityQuaternion, 0, f, spec.isPosition);
          multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
        } else {
          multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
        }
      }
    }
  }

  void _blendPickUp(AnimationMapping &spec, Avatar *avatar) {
    // if (spec.isPosition) avatar->testBlendStrings += "_blendPickUp, "; // test: blend strings.

    Animation *pickUpAnimation = pickUpAnimations[(int)PickUpAnimationIndex::PickUpZelda];
    Animation *pickUpIdleAnimation = pickUpAnimations[(int)PickUpAnimationIndex::PickUpIdleZelda];

    float t2 = avatar->pickUpTime / 1000;
    if (t2 < pickUpAnimation->duration) {
      float *v2 = evaluateInterpolant(pickUpAnimation, spec.index, t2);
      copyValue(spec.dst, v2, spec.isPosition);
    } else {
      float t3 = fmod((t2 - pickUpAnimation->duration), pickUpIdleAnimation->duration);
      float *v2 = evaluateInterpolant(pickUpIdleAnimation, spec.index, t3);
      copyValue(spec.dst, v2, spec.isPosition);
    }
  }

  void _blendFly(AnimationMapping &spec, Avatar *avatar) {
    if (avatar->flyState || (avatar->flyTime >= 0 && avatar->flyTime < 1000)) {
      // if (spec.isPosition) avatar->testBlendStrings += "_blendFly, "; // test: blend strings.

      float t2 = avatar->flyTime / 1000;
      float f = avatar->flyState ? min(CubicBezierEasing::cubicBezier(t2), 1) : (1 - min(CubicBezierEasing::cubicBezier(t2), 1));
      // float f = avatar->flyState ? min(pow(t2, 0.1), 1) : (1 - min(pow(t2, 0.1), 1));
      float *v2 = evaluateInterpolant(singleAnimations[(int)SingleAnimationIndex::Float], spec.index, fmod(t2, singleAnimations[(int)SingleAnimationIndex::Float]->duration));

      interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);

      if (avatar->holdState && spec.isArm) {
        Animation *holdAnimation = holdAnimations[defaultHoldAnimationIndex];
        float t2 = fmod(AnimationMixer::nowS, holdAnimation->duration);
        float *v2 = evaluateInterpolant(holdAnimation, spec.index, t2);
        copyValue(spec.dst, v2, spec.isPosition);
      }
    }
  };

  void _blendLand(AnimationMapping &spec, Avatar *avatar) {
    if (!avatar->landWithMoving) {
      float animationSpeed = 0.75;
      float landTimeS = avatar->landTime / 1000;
      Animation *landingAnimation = landAnimations[(int)LandAnimationIndex::Landing];
      float landingAnimationDuration = landingAnimation->duration / animationSpeed;
      float landFactor = landTimeS / landingAnimationDuration;

      if (landFactor > 0 && landFactor <= 1) {
        // if (spec.isPosition) avatar->testBlendStrings += "_blendLand, "; // test: blend strings.

        float t2 = landTimeS * animationSpeed;
        float *v2 = evaluateInterpolant(landingAnimation, spec.index, t2);

        float f = (landingAnimationDuration - landTimeS) / 0.05; // 0.05 = 3 frames
        f = clamp(f, 0, 1);

        interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
        _clearXZ(spec.dst, spec.isPosition);
      }
    } else {
      float animationSpeed = 0.95;
      float landTimeS = avatar->landTime / 1000;
      Animation *landingAnimation = landAnimations[(int)LandAnimationIndex::Landing2];
      float landingAnimationDuration = landingAnimation->duration / animationSpeed;
      float landFactor = landTimeS / landingAnimationDuration;

      if (landFactor > 0 && landFactor <= 1) {
        // if (spec.isPosition) avatar->testBlendStrings += "_blendLand, "; // test: blend strings.

        float t2 = landTimeS * animationSpeed;
        float *v2 = evaluateInterpolant(landingAnimation, spec.index, t2);

        /* note: Calculating the time since the player landed on the ground. */
        float f3 = landTimeS / 0.1;
        f3 = clamp(f3, 0, 1);

        /* note: Calculating the time remaining until the landing animation is complete. */
        float f2 = (landingAnimationDuration - landTimeS) / 0.15;
        f2 = clamp(f2, 0, 1);

        float f = min(f3, f2);

        interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
        _clearXZ(spec.dst, spec.isPosition);
      }
    }
  }

  void _blendFallLoop(AnimationMapping &spec, Avatar *avatar) {
    if (avatar->fallLoopFactor > 0) {
      // if (spec.isPosition) avatar->testBlendStrings += "_blendFallLoop, "; // test: blend strings.

      float t2 = (avatar->fallLoopTime / 1000);
      float *v2 = evaluateInterpolant(singleAnimations[(int)SingleAnimationIndex::FallLoop], spec.index, t2);
      float f = clamp(t2 / 0.3, 0, 1);

      if (avatar->fallLoopFrom == (int)FallLoopFrom::Jump) {
        copyValue(spec.dst, v2, spec.isPosition);
      } else {
        interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
      }

      _clearXZ(spec.dst, spec.isPosition);
    }
  }

  void _blendSwim(AnimationMapping &spec, Avatar *avatar) {
    if (avatar->swimState) {
      // if (spec.isPosition) avatar->testBlendStrings += "_blendSwim, "; // test: blend strings.

      float swimTimeS = avatar->swimTime / 1000;
      float movementsTimeS = avatar->movementsTime / 1000;

      float t2 = fmod(swimTimeS, singleAnimations[(int)SingleAnimationIndex::Float]->duration);
      float *v2 = evaluateInterpolant(singleAnimations[(int)SingleAnimationIndex::Float], spec.index, t2);

      float t3 = fmod(movementsTimeS * 1, swimAnimations[(int)SwimAnimationIndex::Breaststroke]->duration);
      float *v3 = evaluateInterpolant(swimAnimations[(int)SwimAnimationIndex::Breaststroke], spec.index, t3);

      float t4 = fmod(movementsTimeS * 2, swimAnimations[(int)SwimAnimationIndex::Freestyle]->duration);
      float *v4 = evaluateInterpolant(swimAnimations[(int)SwimAnimationIndex::Freestyle], spec.index, t4);

      float f = clamp(swimTimeS / 0.2, 0, 1);

      if (!spec.isPosition) {
        // // can't use idleWalkFactor & walkRunFactor here, otherwise "Impulsive breaststroke swim animation" will turn into "freestyle animation" when speed is fast,
        // // and will turn into "floating" a little when speed is slow.
        // // interpolateFlat(localQuaternionArr, 0, v3, 0, v4, 0, avatar->walkRunFactor, spec.isPosition);
        // // interpolateFlat(v2, 0, v2, 0, localQuaternionArr, 0, avatar->idleWalkFactor, spec.isPosition);
        interpolateFlat(localQuaternionArr, 0, v3, 0, v4, 0, avatar->sprintFactor, spec.isPosition);
        interpolateFlat(v2, 0, v2, 0, localQuaternionArr, 0, avatar->movementsTransitionFactor, spec.isPosition);
        interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
      } else {
        float liftSwims = 0.05; // lift swims height, prevent head sink in water
        v3[1] += 0.03; // align Swimming.fbx's height to freestyle.fbx
        v3[1] += liftSwims;
        v4[1] += liftSwims;
        interpolateFlat(localQuaternionArr, 0, v3, 0, v4, 0, avatar->sprintFactor, spec.isPosition);
        interpolateFlat(v2, 0, v2, 0, localQuaternionArr, 0, avatar->movementsTransitionFactor, spec.isPosition);
        interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
      }
    }
  }

  void _blendActivate(AnimationMapping &spec, Avatar *avatar) {
    if (avatar->activateTime > 0) {
      // if (spec.isPosition) avatar->testBlendStrings += "_blendActivate, "; // test: blend strings.

      int activateAnimationIndex = avatar->activateAnimationIndex < 0 ? defaultActivateAnimationIndex : avatar->activateAnimationIndex;
      Animation *activateAnimation = activateAnimations[activateAnimationIndex];
      float t2 = fmod((avatar->activateTime / 1000 * activateSpeedFactors[activateAnimationIndex]), activateAnimation->duration);
      float *v2 = evaluateInterpolant(activateAnimation, spec.index, t2);

      float f = avatar->activateTime > 0 ? min(CubicBezierEasing::cubicBezier(t2), 1) : (1 - min(CubicBezierEasing::cubicBezier(t2), 1));
      // float f = avatar->activateTime > 0 ? min(pow(t2, 0.1), 1) : (1 - min(pow(t2, 0.1), 1));

      if (spec.index == BoneIndex::Spine || spec.index == BoneIndex::Chest || spec.index == BoneIndex::UpperChest || spec.index == BoneIndex::Neck || spec.index == BoneIndex::Head) {
        if (!spec.isPosition) {
          multiplyQuaternionsFlat(spec.dst, 0, v2, 0, spec.dst, 0);
        } else {
          interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
        }
      } else {
        if (!spec.isTop) {
          f *= (1 - avatar->idleWalkFactor);
        }
        interpolateFlat(spec.dst, 0, spec.dst, 0, v2, 0, f, spec.isPosition);
      }
    }
  }

  float **AnimationMixer::update(float now, float nowS) {
    // AnimationMixer::now = now; // why can't set, cause idle and dance animatios play very fast ? use file variale instead ?
    AnimationMixer::nowS = nowS;

    for (int i = 0; i < _animationMappings.size(); i++) {
      AnimationMapping spec = _animationMappings[i];

      // if (spec.isPosition) avatar->testBlendStrings = ""; // test: blend strings.

      // note: Use exaclty same early return logic as js version, instead of all cascading, to prevent some bugs. But still want to use all cascading afterwards.
      if (avatar->doubleJumpState) {
        _blendDoubleJump(spec, this->avatar);
      } else if (avatar->jumpState) {
        _blendJump(spec, this->avatar);
      } else if (avatar->sitState) {
        _blendSit(spec, this->avatar);
      } else if (avatar->narutoRunState) {
        _blendNarutoRun(spec, this->avatar);
      } else if (avatar->danceFactor > 0) {
        _blendDance(spec, this->avatar);
      } else if (avatar->emoteFactor > 0) {
        _blendEmote(spec, this->avatar);
      } else if (
        avatar->useAnimationIndex >= 0 ||
        avatar->useAnimationComboIndex >= 0 ||
        avatar->useAnimationEnvelopeIndices.size() > 0
      ) {
        _blendUse(spec, this->avatar);
      } else if (avatar->hurtAnimationIndex >= 0) {
        _blendHurt(spec, this->avatar); // todo: move to highest priority.
      } else if (avatar->aimAnimationIndex >= 0) {
        _blendAim(spec, this->avatar);
      } else if (avatar->unuseAnimationIndex >= 0 && avatar->unuseTime >= 0) {
        _blendUnuse(spec, this->avatar);
      } else if (avatar->holdState) {
        _blendHold(spec, this->avatar);
      } else if (avatar->pickUpState) {
        _blendPickUp(spec, this->avatar);
      } else {
        _handleDefault(spec, this->avatar);
      }

      // note: cascading blending, in order to do transition between all kinds of aniamtions.
      _blendFly(spec, this->avatar);
      _blendFallLoop(spec, this->avatar);
      _blendLand(spec, this->avatar);
      _blendActivate(spec, this->avatar);
      _blendSwim(spec, this->avatar);

      // if (spec.isPosition) std::cout << "testBlendStrings: " << avatar->testBlendStrings << std::endl; // test: blend strings.

      // animationValues[i][0] = spec.dst[0];
      // animationValues[i][1] = spec.dst[1];
      // animationValues[i][2] = spec.dst[2];
      // if (!spec.isPosition) animationValues[i][3] = spec.dst[3];
      animationValues[i] = spec.dst;
    }

    return animationValues;
  }
}