int sgn(double num);

void moveMax(double dist, int timeout, double slowAt);

void set_drive(double inches, int time = 3000, float minSpeed = 0, float maxSpeed=127);

void set_drive_advanced(double inches, int time, float minSpeed, float maxSpeed, float earlyExitRange);

void disruptRingRushBlue();

void MogoSideSoloAWP(bool isBlue);

void disruptRingRush(bool isBlue);

void MogoSideSoloRed();

void MogoSideSoloBlue();

void verySimpleMogo(bool isBlue);

void simpleRing(bool isBlue);

void simpleMogo(bool isBlue);

void newMogoRush(bool isBlue);

void testAuton();

void safeFourRing(bool isBlue);

ASSET(ringRushBlue_txt);
ASSET(ringRushBlueOld_txt);
ASSET(ringRushRed_txt);
ASSET(ringRushRedOld_txt);
