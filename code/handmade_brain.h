/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

struct entity;

//
// NOTE(casey): Brain types
//

struct brain_hero
{
    entity *Head;
    entity *Body;
    entity *Glove;
};

struct brain_monstar
{
    entity *Body;
};

struct brain_familiar
{
    entity *Head;
};

struct brain_snake
{
    entity *Segments[16];
};

//
// NOTE(casey): Brain
//

enum brain_type
{
    Type_brain_hero,

    // NOTE(casey): Test brains!
    Type_brain_snake,
    Type_brain_familiar,
    Type_brain_floaty_thing_for_now,
    Type_brain_monstar,

    Type_brain_count,
};

struct brain_slot 
{
    u16 Type;
    u16 Index;
};
inline b32
IsType(brain_slot Slot, brain_type Type)
{
    b32 Result = ((Slot.Index != 0) && (Slot.Type == Type));
    return(Result);
}

#define BrainSlotFor(type, Member) BrainSlotFor_(Type_##type, &(((type *)0)->Member) - (entity **)0)
#define IndexedBrainSlotFor(type, Member, Index) BrainSlotFor_(Type_##type, (u16)(Index) + (u16)((((type *)0)->Member) - (entity **)0))
inline brain_slot
BrainSlotFor_(brain_type Type, u16 PackValue)
{
    brain_slot Result = {(u16)Type, PackValue};

    return(Result);
}

struct brain_id
{
    u32 Value;
};
inline brain_id NoBrain(void) {brain_id Result = {}; return(Result);}

struct brain
{
    brain_id ID;
    brain_type Type;
    
    union
    {
        entity *Array;
        brain_hero Hero;
        brain_monstar Monstar;
        brain_familiar Familiar;
        brain_snake Snake;
    };
};

enum reserved_brain_id
{
    ReservedBrainID_FirstHero = 1,
    ReservedBrainID_LastHero = (ReservedBrainID_FirstHero + MAX_CONTROLLER_COUNT - 1),
    
    ReservedBrainID_FirstFree,
};
