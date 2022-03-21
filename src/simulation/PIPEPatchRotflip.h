#pragma once

#include "Particle.h"

#include <array>
#include <cstdint>

inline void PIPEPatchRotflip(Particle &part, uint32_t rotflip)
{
    class PatchRotflip
    {
        struct Link
        {
            int xoff, yoff, dir;
        };

        static constexpr std::array<Link, 8> HoFlip(std::array<Link, 8> links)
        {
            for (auto &link : links)
            {
                link.xoff = -link.xoff;
            }
            return links;
        }

        static constexpr std::array<Link, 8> Rotate(std::array<Link, 8> links)
        {
            for (auto &link : links)
            {
                auto yoff = -link.xoff;
                link.xoff = link.yoff;
                link.yoff = yoff;
            }
            return links;
        }

        static constexpr std::array<Link, 8> Identity()
        {
            std::array<Link, 8> identity{};
            int dir = 0;
            for (auto xoff = -1; xoff <= 1; ++xoff)
            {
                for (auto yoff = -1; yoff <= 1; ++yoff)
                {
                    if (xoff || yoff)
                    {
                        identity[dir] = { xoff, yoff, dir };
                        dir += 1;
                    }
                }
            }
            return identity;
        }

        static constexpr std::array<int, 8> Row(std::array<Link, 8> links)
        {
            std::array<int, 8> row{};
            auto identity = Identity();
            for (auto i = 0; i < 8; ++i)
            {
                for (auto j = 0; j < 8; ++j)
                {
                    if (links[i].xoff == identity[j].xoff &&
                        links[i].yoff == identity[j].yoff)
                    {
                        row[links[i].dir] = j;
                    }
                }
            }
            return row;
        }

    public:
        static constexpr std::array<std::array<int, 8>, 8> Generate()
        {
            return {
                Row(                            Identity())    ,
                Row(                     Rotate(Identity()))   ,
                Row(              Rotate(Rotate(Identity())))  ,
                Row(       Rotate(Rotate(Rotate(Identity())))) ,
                Row(HoFlip(                     Identity()))   ,
                Row(HoFlip(              Rotate(Identity())))  ,
                Row(HoFlip(       Rotate(Rotate(Identity())))) ,
                Row(HoFlip(Rotate(Rotate(Rotate(Identity()))))),
            };
        }
    };

    static constexpr std::array<std::array<int, 8>, 8> patchRotflip = PatchRotflip::Generate();
    if (part.tmp & 0x00000100)
    {
        if (part.tmp & 0x00000200) part.tmp = (part.tmp & 0xFFFFE3FF) | (patchRotflip[rotflip][(part.tmp & 0x00001C00) >> 10] << 10);
        if (part.tmp & 0x00002000) part.tmp = (part.tmp & 0xFFFE3FFF) | (patchRotflip[rotflip][(part.tmp & 0x0001C000) >> 14] << 14);
    }
}
