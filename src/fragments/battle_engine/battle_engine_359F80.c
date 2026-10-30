#include "battle_engine.h"

/*debug stuff*/

static const char D_8438D920[0xC] = "PROB %d\n";
static const char D_8438D92C[0x24] = "^^^^^^^^^^POWER += POWER * %2.1f\n";
static const char D_8438D950[0x18] = "^^^^^^^^^CRITICAL %d\n";
static const char D_8438D968[0x10] = "zokuseiFlag %d\n";
static const char D_8438D978[0x4] = "\n\n";
static const char D_8438D97C[0x24] = "******** CkeckCondition **********\n";
static const char D_8438D9A0[0x10] = " MASK_SLEEP \n";
static const char D_8438D9B0[0x10] = " BIT_FREEZE \n";
static const char D_8438D9C0[0x14] = " BIT_SHIMETSUKE \n";
static const char D_8438D9D4[0x10] = " BIT_KIZETSU \n";
static const char D_8438D9E4[0x10] = " BIT_KATAMARU \n";
static const char D_8438D9F4[0xC] = " count4 %d\n";
static const char D_8438DA00[0x14] = " BIT_KONRAN OWARI\n";
static const char D_8438DA14[0x10] = " BIT_KONRAN \n";
static const char D_8438DA24[0x14] = " BIT_KONRAN RND\n";
static const char D_8438DA38[0x24] = " NowSkill %d  kanashibariSkill %d\n";
static const char D_8438DA5C[0x10] = " BIT_PARALYSE \n";
static const char D_8438DA6C[0x10] = " BIT_GAMAN \n";
static const char D_8438DA7C[0x14] = "CLEAR  BIT_GAMAN \n";
static const char D_8438DA90[0x10] = " BIT_ABARERU \n";
static const char D_8438DAA0[0x14] = " BIT_SHIMETSUKE %d\n";
static const char D_8438DAB4[0x10] = " BIT_IKARI \n";
static const char D_8438DAC4[0x4] = "\n\n";
static const char D_8438DAC8[0x24] = "******** Kougeki1 Sart **********\n";
static const char D_8438DAEC[0x10] = " JumpAttack2\n";
static const char D_8438DAFC[0x10] = " JumpAttack3\n";
static const char D_8438DB0C[0x10] = " SpAttackTbl1\n";
static const char D_8438DB1C[0x10] = " JumpAttack4\n";
static const char D_8438DB2C[0x10] = " SpAttackTbl3\n";
static const char D_8438DB3C[0x10] = " JumpAttack5\n";
static const char D_8438DB4C[0x14] = " JumpAttack5 Avoid\n";
static const char D_8438DB60[0x10] = " JumpAttack6\n";
static const char D_8438DB70[0x10] = " --->SHIMETUKE\n";
static const char D_8438DB80[0x10] = " effectPass\n";
static const char D_8438DB90[0xC] = " attack 9\n";
static const char D_8438DB9C[0xC] = " Yubifuri\n";
static const char D_8438DBA8[0x10] = " SpAttackTbl4\n";
static const char D_8438DBB8[0x8] = " Avoid\n";
static const char D_8438DBC0[0xC] = " MESSAGE\n";
static const char D_8438DBCC[0x10] = " SpAttackTbl5\n";
static const char D_8438DBDC[0x10] = " IkariCheck\n";
static const char D_8438DBEC[0x14] = " RENZOKU Count %d\n";

void func_8436F6C0(void) {
}
