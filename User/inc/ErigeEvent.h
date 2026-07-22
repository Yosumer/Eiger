
#ifndef ERIGEVENT_H
#define ERIGEVENT_H
enum  GlobalSignal {
    CASSETTERELEASE_SIG = Q_USER_SIG,
    CASSETTEINDOWN_SIG,    //?²G²°¥[?§Ή¦¨
    MAX_PUB_SIG,
    CASSETTEIN_SIG,
    CASSETTE_RIGHT_ACTIVE_SIG,     //¥k¥ϊ‰m??¨μ¦μ
    CASSETTE_LEFT_ACTIVE_SIG,      //¥ª¥ϊ‰m??¨μ¦μ
    CASS_LOCKED_SIG,        //?¤φ?¦ν
    CASSETTE_PRIM_SIG,      //?²G²°?¥R
    IV_SENSOR_ACTIVE_SIG,   //IV§ύ¥ϊ‰m??¤U­­¦μ
    INFUSION_STOP_SIG,      //Διª`°±¤ξ«H­
    IV_SENSOR_UNACTIVE_SIG,           //IV§ύΞ`¦μ«H­  
    INFUSION_ENABLE_SIG,    //Διª`°ª«Χ?Ύγ«H­
    SELF_CHECK_SIG,
    REFLUX_STOP_SIG,        //¦^¬y±Ζ²G°±¤ξ«H­
    REFLUX_ENABLE_SIG,      //¦^¬y¨Ο―ΰ«H­
    DRAIN_ENABLE_SIG,       //±Ζ²G°±¤ξ«H­
    PRIMING_START_SIG,
    PHACO_START_SIG,
    PRIMING_STOP_SIG,
    SUCTION_STOP_SIG,
    SUCTION_ENABLE_SIG,
    TIMEOUT_SIG,
    MAX_SIG
};

//$declare${Events::ErigeEvt} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${Events::ErigeEvt} ........................................................
typedef struct {
// protected:
    QEvt super;
} ErigeEvt;
//$enddecl${Events::ErigeEvt} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


#endif