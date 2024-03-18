/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

struct sort_entry
{
    r32 SortKey;
    u32 Index;
};

inline u32
SortKeyToU32(r32 SortKey)
{
    // NOTE(casey): We need to turn our 32-bit floating point value
    // into some strictly ascending 32-bit unsigned integer value
    u32 Result = *(u32 *)&SortKey;
    if(Result & 0x80000000)
    {
        Result = ~Result;
    }
    else
    {
        Result |= 0x80000000;
    }
    return(Result);
}

inline void
Swap(sort_entry *A, sort_entry *B)
{
    sort_entry Temp = *B;
    *B = *A;
    *A = Temp;
}

internal void
MergeSort(u32 Count, sort_entry *First, sort_entry *Temp)
{
    if(Count == 1)
    {
        // NOTE(casey): No work to do.
    }
    else if(Count == 2)
    {
        sort_entry *EntryA = First;
        sort_entry *EntryB = First + 1;
        if(EntryA->SortKey > EntryB->SortKey)
        {
            Swap(EntryA, EntryB);
        }
    }
    else
    {
        u32 Half0 = Count / 2;
        u32 Half1 = Count - Half0;

        Assert(Half0 >= 1);
        Assert(Half1 >= 1);

        sort_entry *InHalf0 = First;
        sort_entry *InHalf1 = First + Half0;
        sort_entry *End = First + Count;

        MergeSort(Half0, InHalf0, Temp);
        MergeSort(Half1, InHalf1, Temp);

        sort_entry *ReadHalf0 = InHalf0;
        sort_entry *ReadHalf1 = InHalf1;

        sort_entry *Out = Temp;
        for(u32 Index = 0;
            Index < Count;
            ++Index)
        {
            if(ReadHalf0 == InHalf1)
            {
                *Out++ = *ReadHalf1++;
            }
            else if(ReadHalf1 == End)
            {
                *Out++ = *ReadHalf0++;
            }
            else if(ReadHalf0->SortKey < ReadHalf1->SortKey)
            {
                *Out++ = *ReadHalf0++;
            }
            else
            {
                *Out++ = *ReadHalf1++;
            }            
        }
        Assert(Out == (Temp + Count));
        Assert(ReadHalf0 == InHalf1);
        Assert(ReadHalf1 == End);

        // TODO(casey): Not really necessary if we ping-pong
        for(u32 Index = 0;
            Index < Count;
            ++Index)
        {
            First[Index] = Temp[Index];
        }

#if 0
        sort_entry *ReadHalf0 = First;
        sort_entry *ReadHalf1 = First + Half0;
        sort_entry *End = First + Count;

        // NOTE(casey): Step 1 - Find the first out-of-order pair
        while((ReadHalf0 != ReadHalf1) &&
              (ReadHalf0->SortKey < ReadHalf1->SortKey))
        {
            ++ReadHalf0;
        }

        // NOTE(casey): Step 2 - Swap as many Half1 items in as necessary
        if(ReadHalf0 != ReadHalf1)
        {
            sort_entry CompareWith = *ReadHalf0;
            while((ReadHalf1 != End) && (ReadHalf1->SortKey < CompareWith.SortKey))
            {
                Swap(ReadHalf0++, ReadHalf1++);
            }

            ReadHalf1 = InHalf1;
        }
#endif
    }
}

internal void
BubbleSort(u32 Count, sort_entry *First, sort_entry *Temp)
{
    //
    // NOTE(casey): This is the O(n^2) bubble sort
    //
    for(u32 Outer = 0;
        Outer < Count;
        ++Outer)
    {
        b32 ListIsSorted = true;
        for(u32 Inner = 0;
            Inner < (Count - 1);
            ++Inner)
        {
            sort_entry *EntryA = First + Inner;
            sort_entry *EntryB = EntryA + 1;

            if(EntryA->SortKey > EntryB->SortKey)
            {
                Swap(EntryA, EntryB);
                ListIsSorted = false;
            }
        }

        if(ListIsSorted)
        {
            break;
        }
    }
}

internal void
RadixSort(u32 Count, sort_entry *First, sort_entry *Temp)
{
    sort_entry *Source = First;
    sort_entry *Dest = Temp;
    for(u32 ByteIndex = 0;
        ByteIndex < 32;
        ByteIndex += 8)
    {
        u32 SortKeyOffsets[256] = {};

        // NOTE(casey): First pass - count how many of each key
        for(u32 Index = 0;
            Index < Count;
            ++Index)
        {
            u32 RadixValue = SortKeyToU32(Source[Index].SortKey);
            u32 RadixPiece = (RadixValue >> ByteIndex) & 0xFF;
            ++SortKeyOffsets[RadixPiece];
        }

        // NOTE(casey): Change counts to offsets
        u32 Total = 0;
        for(u32 SortKeyIndex = 0;
            SortKeyIndex < ArrayCount(SortKeyOffsets);
            ++SortKeyIndex)
        {
            u32 Count = SortKeyOffsets[SortKeyIndex];
            SortKeyOffsets[SortKeyIndex] = Total;
            Total += Count;
        }

        // NOTE(casey): Second pass - place elements into the right location
        for(u32 Index = 0;
            Index < Count;
            ++Index)
        {
            u32 RadixValue = SortKeyToU32(Source[Index].SortKey);
            u32 RadixPiece = (RadixValue >> ByteIndex) & 0xFF;
            Dest[SortKeyOffsets[RadixPiece]++] = Source[Index];
        }

        sort_entry *SwapTemp = Dest;
        Dest = Source;
        Source = SwapTemp;
    }
}

struct sort_sprite_bound
{
    r32 YMin;
    r32 YMax;
    r32 ZMax;
    u32 Index;
};

inline b32
IsInFrontOf(sort_sprite_bound *A, sort_sprite_bound *B)
{
    b32 BothZSprites = ((A->YMin != A->YMax) && (B->YMin != B->YMax));
    b32 AIncludesB = ((B->YMin >= A->YMin) && (B->YMin < A->YMax));
    b32 BIncludesA = ((A->YMin >= B->YMin) && (A->YMin < B->YMax));
    
    b32 SortByZ = (BothZSprites || AIncludesB || BIncludesA);
    
    b32 Result = (SortByZ ? (A->ZMax > B->ZMax) : (A->YMin < B->YMin));
    return(Result);
}

inline void
Swap(sort_sprite_bound *A, sort_sprite_bound *B)
{
    sort_sprite_bound Temp = *B;
    *B = *A;
    *A = Temp;
}

internal void
MergeSort(u32 Count, sort_sprite_bound *First, sort_sprite_bound *Temp)
{
    if(Count == 1)
    {
        // NOTE(casey): No work to do.
    }
    else if(Count == 2)
    {
        sort_sprite_bound *EntryA = First;
        sort_sprite_bound *EntryB = First + 1;
        if(IsInFrontOf(EntryB, EntryA))
        {
            Swap(EntryA, EntryB);
        }
    }
    else
    {
        u32 Half0 = Count / 2;
        u32 Half1 = Count - Half0;

        Assert(Half0 >= 1);
        Assert(Half1 >= 1);

        sort_sprite_bound *InHalf0 = First;
        sort_sprite_bound *InHalf1 = First + Half0;
        sort_sprite_bound *End = First + Count;

        MergeSort(Half0, InHalf0, Temp);
        MergeSort(Half1, InHalf1, Temp);

        sort_sprite_bound *ReadHalf0 = InHalf0;
        sort_sprite_bound *ReadHalf1 = InHalf1;

        sort_sprite_bound *Out = Temp;
        for(u32 Index = 0;
            Index < Count;
            ++Index)
        {
            if(ReadHalf0 == InHalf1)
            {
                *Out++ = *ReadHalf1++;
            }
            else if(ReadHalf1 == End)
            {
                *Out++ = *ReadHalf0++;
            }
            else if(IsInFrontOf(ReadHalf0, ReadHalf1))
            {
                *Out++ = *ReadHalf0++;
            }
            else
            {
                *Out++ = *ReadHalf1++;
            }            
        }
        Assert(Out == (Temp + Count));
        Assert(ReadHalf0 == InHalf1);
        Assert(ReadHalf1 == End);

        // TODO(casey): Not really necessary if we ping-pong
        for(u32 Index = 0;
            Index < Count;
            ++Index)
        {
            First[Index] = Temp[Index];
        }
    }
}
