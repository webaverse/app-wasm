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

// ------   note: Start from 0, -1 means null/undefined.

enum class FallLoopFrom {
  Jump
};

// ---------------------------------------------------------

class AnimationDeclaration {
public:
  std::string keyName;
  int index;
  std::string fileName;
};

class AnimationGroupDeclaration {
public:
  std::string groupName;
  int index;
  std::vector<AnimationDeclaration> animationDeclarations;
};

typedef std::vector<AnimationGroupDeclaration> AnimationGroupDeclarations;

// ------

int animationGroupIota = 0;
class AnimationGroupIndexes {
public:
  int Single;
  //
  int Walk;
  int Run;
  int Crouch;
  //
  int Activate;
  int Aim;
  int Dance;
  int Emote;
  int Hold;
  int Hurt;
  int Land;
  int NarutoRun;
  int PickUp;
  int Sit;
  int Swim;
  int Use;
};
AnimationGroupIndexes animationGroupIndexes;

// ---   note: Start from 0, -1 means null/undefined.

int singleAnimationIota = 0;
class SingleAnimationIndexes {
public:
  int Idle;
  int CrouchIdle;
  int Jump;
  int DoubleJump;
  int FallLoop;
  int Float; // note: Have to use upper case. Otherwise will cause error "error: cannot combine with previous 'int' declaration specifier `int float`;" .
};
SingleAnimationIndexes singleAnimationIndexes;

// ---

int walkAnimationIota = 0;
class WalkAnimationIndexes {
public:
  int Forward;
  int Backward;
  int Left;
  int LeftMirror;
  int Right;
  int RightMirror;
};
WalkAnimationIndexes walkAnimationIndexes;

int runAnimationIota = 0;
class RunAnimationIndexes {
public:
  int Forward;
  int Backward;
  int Left;
  int LeftMirror;
  int Right;
  int RightMirror;
};
RunAnimationIndexes runAnimationIndexes;

int crouchAnimationIota = 0;
class CrouchAnimationIndexes {
public:
  int Forward;
  int Backward;
  int Left;
  int LeftMirror;
  int Right;
  int RightMirror;
};
CrouchAnimationIndexes crouchAnimationIndexes;

// ---

int activateAnimationIota = 0;
class ActivateAnimationIndexes {
public:
  int Grab_forward;
  int Grab_down;
  int Grab_up;
  int Grab_left;
  int Grab_right;
  int Pick_up;
};
ActivateAnimationIndexes activateAnimationIndexes;

int aimAnimationIota = 0;
class AimAnimationIndexes {
public:
  int SwordSideIdle;
  int SwordSideSlash;
  int SwordSideSlashStep;
  int SwordTopDownSlash;
  int SwordTopDownSlashStep;
};
AimAnimationIndexes aimAnimationIndexes;

int danceAnimationIota = 0;
class DanceAnimationIndexes {
public:
  int Dansu;
  int Powerup;
};
DanceAnimationIndexes danceAnimationIndexes;

int emoteAnimationIota = 0;
class EmoteAnimationIndexes {
public:
  int Alert;
  int AlertSoft;
  int Angry;
  int AngrySoft;
  int Embarrassed;
  int EmbarrassedSoft;
  int HeadNod;
  int HeadNodSoft;
  int HeadShake;
  int HeadShakeSoft;
  int Sad;
  int SadSoft;
  int Surprise;
  int SurpriseSoft;
  int Victory;
  int VictorySoft;
};
EmoteAnimationIndexes emoteAnimationIndexes;

int holdAnimationIota = 0;
class HoldAnimationIndexes {
public:
  int Pick_up_idle;
};
HoldAnimationIndexes holdAnimationIndexes;

int hurtAnimationIota = 0;
class HurtAnimationIndexes {
public:
  int Pain_back;
  int Pain_arch;
};
HurtAnimationIndexes hurtAnimationIndexes;

int landAnimationIota = 0;
class LandAnimationIndexes {
public:
  int Landing;
  int Landing2;
};
LandAnimationIndexes landAnimationIndexes;

int narutoRunAnimationIota = 0;
class NarutoRunAnimationIndexes {
public:
  int NarutoRun;
};
NarutoRunAnimationIndexes narutoRunAnimationIndexes;

int pickUpAnimationIota = 0;
class PickUpAnimationIndexes {
public:
  int PickUp;
  int PickUpIdle;
  int PickUpThrow;
  int PutDown;
  int PickUpZelda;
  int PickUpIdleZelda;
  int PutDownZelda;
};
PickUpAnimationIndexes pickUpAnimationIndexes;

int sitAnimationIota = 0;
class SitAnimationIndexes {
public:
  int Chair;
  int Saddle;
  int Stand;
};
SitAnimationIndexes sitAnimationIndexes;

int swimAnimationIota = 0;
class SwimAnimationIndexes {
public:
  int Breaststroke;
  int Freestyle;
};
SwimAnimationIndexes swimAnimationIndexes;

int useAnimationIota = 0;
class UseAnimationIndexes {
public:
  int Combo;
  int Slash;
  int Rifle;
  int Pistol;
  int Magic;
  int Eat;
  int Drink;
  int Throw; // note: Have to use uppercase, lower case "throw" will cause key word conflict error "error: expected identifier".
  int PickUpThrow;
  int BowDraw;
  int BowIdle;
  int BowLoose;
  int Pickaxe;
  int SwordSideIdle;
  int SwordSideSlash;
  int SwordSideSlashStep;
  int SwordTopDownSlash;
  int SwordTopDownSlashStep;
};
UseAnimationIndexes useAnimationIndexes;

// ------

AnimationGroupDeclarations declarations = {
  {
    "Single",
    animationGroupIndexes.Single = animationGroupIota++,
    {
      {"Idle", singleAnimationIndexes.Idle = singleAnimationIota++, "idle.fbx"}, // todo: Put `Animation *` here directly ?
      {"CrouchIdle", singleAnimationIndexes.CrouchIdle = singleAnimationIota++, "Crouch Idle.fbx"},
      {"Jump", singleAnimationIndexes.Jump = singleAnimationIota++, "jump.fbx"},
      {"DoubleJump", singleAnimationIndexes.DoubleJump = singleAnimationIota++, "jump_double.fbx"},
      {"FallLoop", singleAnimationIndexes.FallLoop = singleAnimationIota++, "falling.fbx"},
      {"Float", singleAnimationIndexes.Float = singleAnimationIota++, "treading water.fbx"},
    }
  },
  //
  {
    "Walk",
    animationGroupIndexes.Walk = animationGroupIota++,
    {
      {"Forward", walkAnimationIndexes.Forward = walkAnimationIota++, "walking.fbx"},
      {"Backward", walkAnimationIndexes.Backward = walkAnimationIota++, "walking backwards.fbx"},
      {"Left", walkAnimationIndexes.Left = walkAnimationIota++, "left strafe walking.fbx"},
      {"LeftMirror", walkAnimationIndexes.LeftMirror = walkAnimationIota++, "right strafe walking reverse.fbx"},
      {"Right", walkAnimationIndexes.Right = walkAnimationIota++, "right strafe walking.fbx"},
      {"RightMirror", walkAnimationIndexes.RightMirror = walkAnimationIota++, "left strafe walking reverse.fbx"},
    }
  },
  {
    "Run",
    animationGroupIndexes.Run = animationGroupIota++,
    {
      {"Forward", runAnimationIndexes.Forward = runAnimationIota++, "Fast Run.fbx"},
      {"Backward", runAnimationIndexes.Backward = runAnimationIota++, "running backwards.fbx"},
      {"Left", runAnimationIndexes.Left = runAnimationIota++, "left strafe.fbx"},
      {"LeftMirror", runAnimationIndexes.LeftMirror = runAnimationIota++, "right strafe reverse.fbx"},
      {"Right", runAnimationIndexes.Right = runAnimationIota++, "right strafe.fbx"},
      {"RightMirror", runAnimationIndexes.RightMirror = runAnimationIota++, "left strafe reverse.fbx"},
    }
  },
  {
    "Crouch",
    animationGroupIndexes.Crouch = animationGroupIota++,
    {
      {"Forward", crouchAnimationIndexes.Forward = crouchAnimationIota++, "Sneaking Forward.fbx"},
      {"Backward", crouchAnimationIndexes.Backward = crouchAnimationIota++, "Sneaking Forward reverse.fbx"},
      {"Left", crouchAnimationIndexes.Left = crouchAnimationIota++, "Crouched Sneaking Left.fbx"},
      {"LeftMirror", crouchAnimationIndexes.LeftMirror = crouchAnimationIota++, "Crouched Sneaking Right reverse.fbx"},
      {"Right", crouchAnimationIndexes.Right = crouchAnimationIota++, "Crouched Sneaking Right.fbx"},
      {"RightMirror", crouchAnimationIndexes.RightMirror = crouchAnimationIota++, "Crouched Sneaking Left reverse.fbx"},
    }
  },
  //
  {
    "Activate",
    animationGroupIndexes.Activate = animationGroupIota++,
    {
      {"Grab_forward", activateAnimationIndexes.Grab_forward = activateAnimationIota++, "grab_forward.fbx"},
      {"Grab_down", activateAnimationIndexes.Grab_down = activateAnimationIota++, "grab_down.fbx"},
      {"Grab_up", activateAnimationIndexes.Grab_up = activateAnimationIota++, "grab_up.fbx"},
      {"Grab_left", activateAnimationIndexes.Grab_left = activateAnimationIota++, "grab_left.fbx"},
      {"Grab_right", activateAnimationIndexes.Grab_right = activateAnimationIota++, "grab_right.fbx"},
      {"Pick_up", activateAnimationIndexes.Pick_up = activateAnimationIota++, "pick_up.fbx"},
    }
  },
  {
    "Aim",
    animationGroupIndexes.Aim = animationGroupIota++,
    {
      {"SwordSideIdle", aimAnimationIndexes.SwordSideIdle = aimAnimationIota++, "sword_idle_side.fbx"},
      {"SwordSideSlash", aimAnimationIndexes.SwordSideSlash = aimAnimationIota++, "sword_side_slash.fbx"},
      {"SwordSideSlashStep", aimAnimationIndexes.SwordSideSlashStep = aimAnimationIota++, "sword_side_slash_step.fbx"},
      {"SwordTopDownSlash", aimAnimationIndexes.SwordTopDownSlash = aimAnimationIota++, "sword_topdown_slash.fbx"},
      {"SwordTopDownSlashStep", aimAnimationIndexes.SwordTopDownSlashStep = aimAnimationIota++, "sword_topdown_slash_step.fbx"},
    }
  },
  {
    "Dance",
    animationGroupIndexes.Dance = animationGroupIota++,
    {
      {"Dansu", danceAnimationIndexes.Dansu = danceAnimationIota++, "Hip Hop Dancing.fbx"},
      {"Powerup", danceAnimationIndexes.Powerup = danceAnimationIota++, "powerup.fbx"},
    }
  },
  {
    "Emote",
    animationGroupIndexes.Emote = animationGroupIota++,
    {
      {"Alert", emoteAnimationIndexes.Alert = emoteAnimationIota++, "alert.fbx"},
      {"AlertSoft", emoteAnimationIndexes.AlertSoft = emoteAnimationIota++, "alert_soft.fbx"},
      {"Angry", emoteAnimationIndexes.Angry = emoteAnimationIota++, "angry.fbx"},
      {"AngrySoft", emoteAnimationIndexes.AngrySoft = emoteAnimationIota++, "angry_soft.fbx"},
      {"Embarrassed", emoteAnimationIndexes.Embarrassed = emoteAnimationIota++, "embarrassed.fbx"},
      {"EmbarrassedSoft", emoteAnimationIndexes.EmbarrassedSoft = emoteAnimationIota++, "embarrassed_soft.fbx"},
      {"HeadNod", emoteAnimationIndexes.HeadNod = emoteAnimationIota++, "head_nod.fbx"},
      {"HeadNodSoft", emoteAnimationIndexes.HeadNodSoft = emoteAnimationIota++, "head_nod_single.fbx"},
      {"HeadShake", emoteAnimationIndexes.HeadShake = emoteAnimationIota++, "head_shake.fbx"},
      {"HeadShakeSoft", emoteAnimationIndexes.HeadShakeSoft = emoteAnimationIota++, "head_shake_single.fbx"},
      {"Sad", emoteAnimationIndexes.Sad = emoteAnimationIota++, "sad.fbx"},
      {"SadSoft", emoteAnimationIndexes.SadSoft = emoteAnimationIota++, "sad_soft.fbx"},
      {"Surprise", emoteAnimationIndexes.Surprise = emoteAnimationIota++, "surprise.fbx"},
      {"SurpriseSoft", emoteAnimationIndexes.SurpriseSoft = emoteAnimationIota++, "surprise_soft.fbx"},
      {"Victory", emoteAnimationIndexes.Victory = emoteAnimationIota++, "victory.fbx"},
      {"VictorySoft", emoteAnimationIndexes.VictorySoft = emoteAnimationIota++, "victory_soft.fbx"},
    }
  },
  {
    "Hold",
    animationGroupIndexes.Hold = animationGroupIota++,
    {
      {"Pick_up_idle", holdAnimationIndexes.Pick_up_idle = holdAnimationIota++, "pick_up_idle.fbx"},
    }
  },
  {
    "Hurt",
    animationGroupIndexes.Hurt = animationGroupIota++,
    {
      {"Pain_back", hurtAnimationIndexes.Pain_back = hurtAnimationIota++, "pain_back.fbx"},
      {"Pain_arch", hurtAnimationIndexes.Pain_arch = hurtAnimationIota++, "pain_arch.fbx"},
    }
  },
  {
    "Land",
    animationGroupIndexes.Land = animationGroupIota++,
    {
      {"Landing", landAnimationIndexes.Landing = landAnimationIota++, "landing.fbx"},
      {"Landing2", landAnimationIndexes.Landing2 = landAnimationIota++, "landing 2.fbx"},
    }
  },
  {
    "NarutoRun",
    animationGroupIndexes.NarutoRun = animationGroupIota++,
    {
      {"NarutoRun", narutoRunAnimationIndexes.NarutoRun = narutoRunAnimationIota++, "naruto run.fbx"},
    }
  },
  {
    "PickUp",
    animationGroupIndexes.PickUp = animationGroupIota++,
    {
      {"PickUp", pickUpAnimationIndexes.PickUp = pickUpAnimationIota++, "pick_up.fbx"},
      {"PickUpIdle", pickUpAnimationIndexes.PickUpIdle = pickUpAnimationIota++, "pick_up_idle.fbx"},
      {"PickUpThrow", pickUpAnimationIndexes.PickUpThrow = pickUpAnimationIota++, "pick_up_throw.fbx"},
      {"PutDown", pickUpAnimationIndexes.PutDown = pickUpAnimationIota++, "put_down.fbx"},
      {"PickUpZelda", pickUpAnimationIndexes.PickUpZelda = pickUpAnimationIota++, "pick_up_zelda.fbx"},
      {"PickUpIdleZelda", pickUpAnimationIndexes.PickUpIdleZelda = pickUpAnimationIota++, "pick_up_idle_zelda.fbx"},
      {"PutDownZelda", pickUpAnimationIndexes.PutDownZelda = pickUpAnimationIota++, "put_down_zelda.fbx"},
    }
  },
  {
    "Sit",
    animationGroupIndexes.Sit = animationGroupIota++,
    {
      {"Chair", sitAnimationIndexes.Chair = sitAnimationIota++, "sitting idle.fbx"},
      {"Saddle", sitAnimationIndexes.Saddle = sitAnimationIota++, "sitting idle.fbx"},
      {"Stand", sitAnimationIndexes.Stand = sitAnimationIota++, "Skateboarding.fbx"},
    }
  },
  {
    "Swim",
    animationGroupIndexes.Swim = animationGroupIota++,
    {
      {"Breaststroke", swimAnimationIndexes.Breaststroke = swimAnimationIota++, "Swimming.fbx"},
      {"Freestyle", swimAnimationIndexes.Freestyle = swimAnimationIota++, "freestyle.fbx"},
    }
  },
  {
    "Use",
    animationGroupIndexes.Use = animationGroupIota++,
    {
      {"Combo", useAnimationIndexes.Combo = useAnimationIota++, "One Hand Sword Combo.fbx"},
      {"Slash", useAnimationIndexes.Slash = useAnimationIota++, "sword and shield slash.fbx"},
      {"Rifle", useAnimationIndexes.Rifle = useAnimationIota++, "Rifle Aiming Idle.fbx"},
      {"Pistol", useAnimationIndexes.Pistol = useAnimationIota++, "Pistol Aiming Idle.fbx"},
      {"Magic", useAnimationIndexes.Magic = useAnimationIota++, "magic cast.fbx"},
      {"Eat", useAnimationIndexes.Eat = useAnimationIota++, "eating.fbx"},
      {"Drink", useAnimationIndexes.Drink = useAnimationIota++, "drinking.fbx"},
      {"Throw", useAnimationIndexes.Throw = useAnimationIota++, "pick_up_throw.fbx"},
      {"PickUpThrow", useAnimationIndexes.PickUpThrow = useAnimationIota++, "pick_up_throw.fbx"},
      {"BowDraw", useAnimationIndexes.BowDraw = useAnimationIota++, "bow draw.fbx"},
      {"BowIdle", useAnimationIndexes.BowIdle = useAnimationIota++, "bow idle.fbx"},
      {"BowLoose", useAnimationIndexes.BowLoose = useAnimationIota++, "bow loose.fbx"},
      {"Pickaxe", useAnimationIndexes.Pickaxe = useAnimationIota++, "pickaxe_swing.fbx"},
      {"SwordSideIdle", useAnimationIndexes.SwordSideIdle = useAnimationIota++, "sword_idle_side.fbx"},
      {"SwordSideSlash", useAnimationIndexes.SwordSideSlash = useAnimationIota++, "sword_side_slash.fbx"},
      {"SwordSideSlashStep", useAnimationIndexes.SwordSideSlashStep = useAnimationIota++, "sword_side_slash_step.fbx"},
      {"SwordTopDownSlash", useAnimationIndexes.SwordTopDownSlash = useAnimationIota++, "sword_topdown_slash.fbx"},
      {"SwordTopDownSlashStep", useAnimationIndexes.SwordTopDownSlashStep = useAnimationIota++, "sword_topdown_slash_step.fbx"},
    }
  },
};