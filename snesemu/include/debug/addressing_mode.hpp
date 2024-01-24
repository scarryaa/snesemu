#ifndef ADDRESSING_MODE_HPP
#define ADDRESSING_MODE_HPP

enum AddressingMode
{
    kAbsolute,
    kImmediate8,
    kImmediate16,
    kImmediate,
    kAbsoluteLong,
    kAccumulator,
    kBranch,
    kAbsoluteIndexedX,
    kAbsoluteIndexedY,
    kAbsoluteLongIndexedX,
    kAbsoluteIndirect,
    kAbsoluteIndirectLong,
    kAbsoluteIndexedIndirectX,
    kLong,
    kDirectPage,
    kDirectPageIndexedX,
    kDirectPageIndexedY,
    kDirectPageIndirect,
    kDirectPageIndirectLong,
    kDirectPageIndirectX,
    kDirectPageIndirectIndexedY,
    kDirectPageIndirectLongIndexedY,
    kStackRelative,
    kStackRelativeIndirectIndexedY,
    kBlockMove,
    kRelative,
    kRelativeLong,
    kNone
};

#endif