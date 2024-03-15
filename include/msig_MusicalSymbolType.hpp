#pragma once
#ifndef msig_MusicalSymbolType_hpp
#define msig_MusicalSymbolType_hpp

namespace msig {
enum MusicalSymbolType {
    // 악상기호 레벨
    MSID_LV0 = 0x00,
    MSID_LV1 = 0x01,
    MSID_LV2 = 0x02,
    MSID_LV3 = 0x03,    // 임시
    MSID_LV4 = 0x04,    // 임시
    
    // 악상기호 타입
    MSID_LINE           = 0x00,
    MSID_MEASURE        = 0x10,
    MSID_CLEF           = 0x20,
    MSID_KEY            = 0x30,
    MSID_TIME           = 0x40,
    MSID_NOTE           = 0x50,
    MSID_REST           = 0x60,
    MSID_REPETITION     = 0x70,
    MSID_OCTAVE         = 0x80,
    MSID_ACCIDENTAL     = 0x90,
    MSID_DYNAMIC        = 0xa0,
    MSID_ARTICULATION   = 0xb0,
    MSID_ORNAMENT       = 0xc0,
};
}

#endif /* msig_MusicalSymbolType_hpp */
