#include "brarchive.h"

namespace rpr {
RPR_USING;
// BrArch main

bool BrArch::load(const string &file) {
    using ios = std::ios;
    this->clear();

    ifstream fin(file, ios::binary);
    if (!fin.is_open()) return false;

    fin.seekg(0, ios::end);
    auto file_size = fin.tellg();
    if (file_size < br_base::header_len) return false;
    fin.seekg(0, ios::beg);

    string buffer(file_size, '\0');
    if (!fin.read(buffer.data(), file_size)) {
        fin.close();
        return false;
    }
    fin.close();

    std::istringstream iss(std::move(buffer), ios::binary);
    if (!iss) return false;
    iss.seekg(0, ios::beg);

    // header
    uint64_t magic = 0;
    uint32_t elem_size = 0;
    iss.read(reinterpret_cast<char*>(&magic),           sizeof(uint64_t)); if (magic != br_base::magic) return false;
    iss.read(reinterpret_cast<char*>(&elem_size),       sizeof(uint32_t));
    iss.read(reinterpret_cast<char*>(&format_version_), sizeof(uint32_t));

    // elems
    elems_.resize(elem_size);
    vector<uint32_t> elem_offsets(elem_size);
    for (uint32_t i = 0; i < elem_size; ++i) {
        elem_t &e = elems_[i];

        uint8_t name_len = 0;
        iss.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));
        e.name.resize(br_base::name_len);
        iss.read(reinterpret_cast<char*>(e.name.data()), br_base::name_len);
        e.name.resize(name_len);

        uint32_t elem_off, elem_len;
        iss.read(reinterpret_cast<char*>(&elem_off), sizeof(uint32_t));
        iss.read(reinterpret_cast<char*>(&elem_len), sizeof(uint32_t));
        elem_offsets[i] = elem_off;
        e.data.resize(elem_len);
    }

    // data
    iss.seekg(br_base::header_len + 0x100 * elem_size, ios::beg);
    auto cur_pos = iss.tellg();
    for (uint32_t i = 0; i < elem_size; ++i) {
        iss.seekg(cur_pos + std::streamoff(elem_offsets[i]), ios::beg);
        iss.read(reinterpret_cast<char*>(elems_[i].data.data()), elems_[i].data.size());
    }

    return true;
}

void BrArch::dump(ostream &os) const {
    if (os.fail()) return;

    // header
    const auto elem_size = elems_.size();
    os.write(reinterpret_cast<const char*>(&br_base::magic),  sizeof(uint64_t));
    os.write(reinterpret_cast<const char*>(&elem_size),       sizeof(uint32_t));
    os.write(reinterpret_cast<const char*>(&format_version_), sizeof(uint32_t));

    // elems meta
    uint32_t eo = 0, el = 0;
    byte_t meta_buffer[0x100];
    for (auto &e : elems_) {
        memset(meta_buffer, 0, sizeof(meta_buffer));
        uint8_t name_len = strnlen(e.name.c_str(), br_base::name_len);
        el = e.data.size();

        meta_buffer[0] = name_len;
        memcpy(&meta_buffer[0x01], e.name.c_str(), name_len);
        memcpy(&meta_buffer[0xF8], &eo, sizeof(uint32_t));
        memcpy(&meta_buffer[0xFC], &el, sizeof(uint32_t));

        os.write(reinterpret_cast<const char*>(meta_buffer), sizeof(meta_buffer));
        eo += el;
    }

    // data
    for (auto &e : elems_) {
        os.write(reinterpret_cast<const char*>(e.data.data()), e.data.size());
    }
}

string BrArch::dump() const {
    std::stringstream ss;
    dump(ss);
    return ss.str();
}

} // namespace rpr
