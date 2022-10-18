#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <filesystem>
#include <bit>

#define META_EOF 0x0000
#define META_REALIZEPALETTE 0x0035
#define META_SETPALENTRIES 0x0037
#define META_SETBKMODE 0x0102
#define META_SETMAPMODE 0x0103
#define META_SETROP2 0x0104
#define META_SETRELABS 0x0105
#define META_SETPOLYFILLMODE 0x0106
#define META_SETSTRETCHBLTMODE 0x0107
#define META_SETTEXTCHAREXTRA 0x0108
#define META_RESTOREDC 0x0127
#define META_RESIZEPALETTE 0x0139
#define META_DIBCREATEPATTERNBRUSH 0x0142
#define META_SETLAYOUT 0x0149
#define META_SETBKCOLOR 0x0201
#define META_SETTEXTCOLOR 0x0209
#define META_OFFSETVIEWPORTORG 0x0211
#define META_LINETO 0x0213
#define META_MOVETO 0x0214
#define META_OFFSETCLIPRGN 0x0220
#define META_FILLREGION 0x0228
#define META_SETMAPPERFLAGS 0x0231
#define META_SELECTPALETTE 0x0234
#define META_POLYGON 0x0324
#define META_POLYLINE 0x0325
#define META_SETTEXTJUSTIFICATION 0x020A
#define META_SETWINDOWORG 0x020B
#define META_SETWINDOWEXT 0x020C
#define META_SETVIEWPORTORG 0x020D
#define META_SETVIEWPORTEXT 0x020E
#define META_OFFSETWINDOWORG 0x020F
#define META_SCALEWINDOWEXT 0x0410
#define META_SCALEVIEWPORTEXT 0x0412
#define META_EXCLUDECLIPRECT 0x0415
#define META_INTERSECTCLIPRECT 0x0416
#define META_ELLIPSE 0x0418
#define META_FLOODFILL 0x0419
#define META_FRAMEREGION 0x0429
#define META_ANIMATEPALETTE 0x0436
#define META_TEXTOUT 0x0521
#define META_POLYPOLYGON 0x0538
#define META_EXTFLOODFILL 0x0548
#define META_RECTANGLE 0x041B
#define META_SETPIXEL 0x041F
#define META_ROUNDRECT 0x061C
#define META_PATBLT 0x061D
#define META_SAVEDC 0x001E
#define META_PIE 0x081A
#define META_STRETCHBLT 0x0B23
#define META_ESCAPE 0x0626
#define META_INVERTREGION 0x012A
#define META_PAINTREGION 0x012B
#define META_SELECTCLIPREGION 0x012C
#define META_SELECTOBJECT 0x012D
#define META_SETTEXTALIGN 0x012E
#define META_ARC 0x0817
#define META_CHORD 0x0830
#define META_BITBLT 0x0922
#define META_EXTTEXTOUT 0x0a32
#define META_SETDIBTODEV 0x0d33
#define META_DIBBITBLT 0x0940
#define META_DIBSTRETCHBLT 0x0b41
#define META_STRETCHDIB 0x0f43
#define META_DELETEOBJECT 0x01f0
#define META_CREATEPALETTE 0x00f7
#define META_CREATEPATTERNBRUSH 0x01F9
#define META_CREATEPENINDIRECT 0x02FA
#define META_CREATEFONTINDIRECT 0x02FB
#define META_CREATEBRUSHINDIRECT 0x02FC
#define META_CREATEREGION 0x06FF
#define r64 *reinterpret_cast<uint64_t*>(bptr); bptr += 8;
#define r32 *reinterpret_cast<uint32_t*>(bptr); bptr += 4;
#define r16 *reinterpret_cast<uint16_t*>(bptr); bptr += 2;
#define r8 *reinterpret_cast<uint8_t*>(bptr); bptr += 1;

#define err(x) std::cout << x << std::endl; return 1;
#define m(x) std::cout << x << std::endl;

#define chk(x, expected, byte) uint16_t z = byte; \
        if (z == expected) { \
            m(#x " fine"); \
        } else { \
            err(#x " bad: " << z); \
        }

static std::vector<uint8_t> ReadAllBytes(std::string filename) {
    std::ifstream ifs(filename, std::ios::binary|std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    if (pos == 0) {
        return std::vector<uint8_t>{};
    }

    std::vector<uint8_t>  result(pos);

    ifs.seekg(0, std::ios::beg);
    ifs.read(reinterpret_cast<char*>(&result[0]), pos);
    ifs.close();
    return result;
}

static std::string get_record(uint16_t rec) {
    switch (rec) {
        case 0x0000: return "META_EOF";
        case 0x0035: return "META_REALIZEPALETTE";
        case 0x0037: return "META_SETPALENTRIES";
        case 0x0102: return "META_SETBKMODE";
        case 0x0103: return "META_SETMAPMODE";
        case 0x0104: return "META_SETROP2";
        case 0x0105: return "META_SETRELABS";
        case 0x0106: return "META_SETPOLYFILLMODE";
        case 0x0107: return "META_SETSTRETCHBLTMODE";
        case 0x0108: return "META_SETTEXTCHAREXTRA";
        case 0x0127: return "META_RESTOREDC";
        case 0x0139: return "META_RESIZEPALETTE";
        case 0x0142: return "META_DIBCREATEPATTERNBRUSH";
        case 0x0149: return "META_SETLAYOUT";
        case 0x0201: return "META_SETBKCOLOR";
        case 0x0209: return "META_SETTEXTCOLOR";
        case 0x0211: return "META_OFFSETVIEWPORTORG";
        case 0x0213: return "META_LINETO";
        case 0x0214: return "META_MOVETO";
        case 0x0220: return "META_OFFSETCLIPRGN";
        case 0x0228: return "META_FILLREGION";
        case 0x0231: return "META_SETMAPPERFLAGS";
        case 0x0234: return "META_SELECTPALETTE";
        case 0x0324: return "META_POLYGON";
        case 0x0325: return "META_POLYLINE";
        case 0x020A: return "META_SETTEXTJUSTIFICATION";
        case 0x020B: return "META_SETWINDOWORG";
        case 0x020C: return "META_SETWINDOWEXT";
        case 0x020D: return "META_SETVIEWPORTORG";
        case 0x020E: return "META_SETVIEWPORTEXT";
        case 0x020F: return "META_OFFSETWINDOWORG";
        case 0x0410: return "META_SCALEWINDOWEXT";
        case 0x0412: return "META_SCALEVIEWPORTEXT";
        case 0x0415: return "META_EXCLUDECLIPRECT";
        case 0x0416: return "META_INTERSECTCLIPRECT";
        case 0x0418: return "META_ELLIPSE";
        case 0x0419: return "META_FLOODFILL";
        case 0x0429: return "META_FRAMEREGION";
        case 0x0436: return "META_ANIMATEPALETTE";
        case 0x0521: return "META_TEXTOUT";
        case 0x0538: return "META_POLYPOLYGON";
        case 0x0548: return "META_EXTFLOODFILL";
        case 0x041B: return "META_RECTANGLE";
        case 0x041F: return "META_SETPIXEL";
        case 0x061C: return "META_ROUNDRECT";
        case 0x061D: return "META_PATBLT";
        case 0x001E: return "META_SAVEDC";
        case 0x081A: return "META_PIE";
        case 0x0B23: return "META_STRETCHBLT";
        case 0x0626: return "META_ESCAPE";
        case 0x012A: return "META_INVERTREGION";
        case 0x012B: return "META_PAINTREGION";
        case 0x012C: return "META_SELECTCLIPREGION";
        case 0x012D: return "META_SELECTOBJECT";
        case 0x012E: return "META_SETTEXTALIGN";
        case 0x0817: return "META_ARC";
        case 0x0830: return "META_CHORD";
        case 0x0922: return "META_BITBLT";
        case 0x0a32: return "META_EXTTEXTOUT";
        case 0x0d33: return "META_SETDIBTODEV";
        case 0x0940: return "META_DIBBITBLT";
        case 0x0b41: return "META_DIBSTRETCHBLT";
        case 0x0f43: return "META_STRETCHDIB";
        case 0x01f0: return "META_DELETEOBJECT";
        case 0x00f7: return "META_CREATEPALETTE";
        case 0x01F9: return "META_CREATEPATTERNBRUSH";
        case 0x02FA: return "META_CREATEPENINDIRECT";
        case 0x02FB: return "META_CREATEFONTINDIRECT";
        case 0x02FC: return "META_CREATEBRUSHINDIRECT";
        case 0x06FF: return "META_CREATEREGION";
        default: throw std::runtime_error("UNKNOWN RECORD!!!!");
    }
}

struct Command {
    std::string Name;
    size_t Start;
    size_t End;
    bool Deleted = false;
};

int main(int argc, char** argv) {
    if (argc != 2)
        return 1;
    std::string path = argv[1];
    std::vector<uint8_t> bytes = ReadAllBytes(path);
    std::vector<Command> commands;
    std::cout << "reading " << bytes.size() << " bytes" << std::endl;
    uint8_t* const bptr_start = bytes.data();
    uint8_t* bptr = bytes.data();
    uint16_t numobjects = 0;
    uint32_t mhsize = 0;
    uint32_t maxsize = 0;
    {
        uint32_t chk = r32;
        if (chk == 0x9AC6CDD7) {
            m("checksum fine");
        } else {
            err("checksum bad: " << chk);
            return 1;
        }
    }
    {
        chk("HWmf", 0, r16);
    }
    {
        uint16_t x = r16;
        uint16_t y = r16;
        uint16_t r = r16;
        uint16_t b = r16;
        m("header bbox: (" << x << ", " << y << ") - (" << r << ", " << b << ")");
    }
    {
        uint16_t inch = r16;
        m("inch: " << inch);
    }
    {
        chk("reserved", 0, r32);
    }
    {
        // chksum
        r16;
    }
    // META_HEADER
    {
        chk("type", 1, r16);
    }
    {
        // headersize
        r16;
    }
    {
        chk("version", 0x300, r16);
    }
    {
        mhsize = r32;
    }
    {
        numobjects = r16;
        m("Number of objects: " << numobjects);
    }
    {
        maxsize = r32;
        m("max size: " << maxsize);
    }
    {
        chk("unused", 0, r16);
    }
    // start of objects
    while(bptr) {
        auto offset = bptr - bptr_start;
        uint32_t size = r32;
        uint16_t record_type = r16;
        if (record_type == 0) {
            m("Hit META_EOF");
            break;
        }
        auto str = get_record(record_type);
        Command c = {.Name = str, .Start = offset, .End = offset + (size * 2)};
        commands.push_back(c);
        m("Got record: " << str << " at offset: " << offset);
        switch (record_type) {
            case META_SETMAPMODE: {
                uint16_t map = r16;
                m("mapmode: " << map);
                break;
            }
            case META_SETWINDOWORG: {
                int x = r16;
                int y = r16;
                m("windoworg: " << x << ", " << y);
                break;
            }
            case META_SETWINDOWEXT: {
                int x = r16;
                int y = r16;
                m("windowext: " << x << ", " << y);
                break;
            }
            case META_SETTEXTCOLOR: {
                auto ref = r32;
                m("textcolorref: " << ref);
                break;
            }
            case META_SETBKCOLOR: {
                auto ref = r32;
                m("bkcolorref: " << ref);
                break;
            }
            case META_DIBSTRETCHBLT: {
                auto raster = r32;
                auto srch = r16;
                auto srcw = r16;
                auto srcy = r16;
                auto srcx = r16;
                auto desth = r16;
                auto destw = r16;
                auto desty = r16;
                auto destx = r16;
                bool has_source_dib = (size != (record_type >> 8) + 3);
                m("has source device independent bitmap: " << has_source_dib);
                m("raster operation: " << raster);
                if (!has_source_dib)
                    throw std::runtime_error("dibstretchblt w/o source record is unhandled");
                uint32_t bmpsize = (size * 2) - 4 - 2 - 20;
                bptr += bmpsize;
                m("skipped " << bmpsize << " bmp bytes");
                break;
            }
            case META_ESCAPE: {
                auto esc_func = r16;
                auto byte_c = r16;
                m("escape func: " << esc_func);
                if (byte_c & 1)
                    byte_c += 1;
                bptr += byte_c;
                break;
            }
            default: {
                m("skipped unhandled record: " << str);
                bptr += (size * 2) - 4 -2;
                break;
            }
        }
    }
    while (true) {
        m("\ncommands: l (list), d (delete), r (restore), s (save as), q (quit)\n\nyour command: ");
        char ch;
        std::cin >> ch;
        std::cout << std::endl;
        switch (ch) {
            case 'l': {
                for (size_t i = 0; i < commands.size(); i++) {
                    bool deleted = commands[i].Deleted;
                    if (deleted) {
                        std::cout << "\e[9m";
                    }
                    std::cout << i << ": " << commands[i].Name;
                    if (deleted) {
                        std::cout << "\e[0m";
                    }
                    std::cout << std::endl;
                }
                break;
            }
            case 'd': {
                std::cout << "What index to delete? (c to cancel)" << std::endl;
                std::string index;
                std::cin >> index;
                if (index == "c")
                    break;
                int indexn = -1;
                try { indexn = std::stoi(index); } catch(...) { std::cout << "bad index! cancelling delete..." << std::endl; break; }
                Command& c = commands[indexn];
                c.Deleted = true;
                break;
            }
            case 'r': {
                std::cout << "What index to restore? (c to cancel)" << std::endl;
                std::string index;
                std::cin >> index;
                if (index == "c")
                    break;
                int indexn = -1;
                try { indexn = std::stoi(index); } catch(...) { std::cout << "bad index! cancelling restore..." << std::endl; break; }
                Command& c = commands[indexn];
                c.Deleted = false;
                break;
            }
            case 's': {
                std::filesystem::path p(path);
                std::string dir = p.parent_path().string();
                std::string name = p.filename().string();
                std::string newpath;
                int additional_num = 0;
                while (true) {
                    newpath = dir + "/" + name + std::to_string(additional_num) + ".wmf";
                    if (std::filesystem::exists(newpath))
                        additional_num++;
                    else
                        break;
                }
                std::vector<uint8_t> bytes_copy = bytes;
                for (int i = commands.size() - 1; i >= 0; i--) {
                    auto c = commands[i];
                    if (c.Deleted) {
                        bytes_copy.erase(bytes_copy.begin() + c.Start, bytes_copy.begin() + c.End);
                    }
                }
                std::ofstream fout(newpath, std::ios::out | std::ios::binary);
                fout.write((char*)&bytes_copy[0], bytes_copy.size());
                fout.close();
                std::cout << "saved to " << newpath << std::endl;
                break;
            }
            case 'q': {
                return 0;
            }
            default: {
                m("bad command!");
                break;
            }
        }
    }
}