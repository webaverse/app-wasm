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

// ------

enum class AnimationGroupIndex {
  Single,
  //
  Walk,
  Run,
  Crouch,
  //
  Activate,
  Aim,
  Dance,
  Emote,
  Hold,
  Hurt,
  Land,
  NarutoRun,
  PickUp,
  Sit,
  Swim,
  Use
};

// ---   note: Start from 0, -1 means null/undefined.

enum class SingleAnimationIndex {
  Idle,
  CrouchIdle,
  Jump,
  DoubleJump,
  FallLoop,
  Float
};

// ---

enum class WalkAnimationIndex {
  Forward,
  Backward,
  Left,
  LeftMirror,
  Right,
  RightMirror
};

enum class RunAnimationIndex {
  Forward,
  Backward,
  Left,
  LeftMirror,
  Right,
  RightMirror
};

enum class CrouchAnimationIndex {
  Forward,
  Backward,
  Left,
  LeftMirror,
  Right,
  RightMirror
};

// ---

enum class ActivateAnimationIndex {
  Grab_forward,
  Grab_down,
  Grab_up,
  Grab_left,
  Grab_right,
  Pick_up
};

enum class AimAnimationIndex {
  SwordSideIdle,
  SwordSideSlash,
  SwordSideSlashStep,
  SwordTopDownSlash,
  SwordTopDownSlashStep
};

enum class DanceAnimationIndex {
  Dansu,
  Powerup
};

enum class EmoteAnimationIndex {
  Alert,
  AlertSoft,
  Angry,
  AngrySoft,
  Embarrassed,
  EmbarrassedSoft,
  HeadNod,
  HeadNodSoft,
  HeadShake,
  HeadShakeSoft,
  Sad,
  SadSoft,
  Surprise,
  SurpriseSoft,
  Victory,
  VictorySoft
};

enum class HoldAnimationIndex {
  Pick_up_idle
};

enum class HurtAnimationIndex {
  Pain_back,
  Pain_arch
};

enum class LandAnimationIndex {
  Landing,
  Landing2
};

enum class NarutoRunAnimationIndex {
  NarutoRun
};

enum class PickUpAnimationIndex {
  PickUp,
  PickUpIdle,
  PickUpThrow,
  PutDown,
  PickUpZelda,
  PickUpIdleZelda,
  PutDownZelda
};

enum class SitAnimationIndex {
  Chair,
  Saddle,
  Stand
};

enum class SwimAnimationIndex {
  Breaststroke,
  Freestyle
};

enum class UseAnimationIndex {
  Combo,
  Slash,
  Rifle,
  Pistol,
  Magic,
  Eat,
  Drink,
  Throw, // note: Have to use uppercase, lower case "throw" will cause key word conflict error "error: expected identifier". // Can use lower case "throw" with enum class ? Still can't.
  PickUpThrow,
  BowDraw,
  BowIdle,
  BowLoose,
  Pickaxe,
  SwordSideIdle,
  SwordSideSlash,
  SwordSideSlashStep,
  SwordTopDownSlash,
  SwordTopDownSlashStep
};

// ------   note: Start from 0, -1 means null/undefined.

enum class FallLoopFrom {
  Jump
};

// ---------------------------------------------------------

class AnimationGroupDeclaration {
public:
  int index;
  std::string groupName;
  std::vector<std::string> animationFileNames;
};