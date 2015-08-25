#ifndef _BYTES_ALIGNER_H__
#define _BYTES_ALIGNER_H__

#include <string>
#include <vector>

#include <stdlib.h>
#include <assert.h>




// Align to 4bytes boundary.
class BytesAligner {
public:
    // T must have data() and []
    template<typename T> void add(const std::string &tag, const T &src) {
        if (src.empty())
            return;
        Item tmp;
        tmp.tag     = tag;
        tmp.data    = reinterpret_cast<const uint8_t*>(src.data());
        tmp.start   = _last.start + _last.length + _last.padding;
        tmp.length  = sizeof(src[0]) * src.size();
        tmp.padding = (tmp.length % 4 == 0) ? 0 : 4 - (tmp.length % 4);
        _last = tmp;
        _data.push_back(std::move(tmp));
    }

    size_t get_start  (const std::string &tag) const { return this->_get_item(tag)->start; }
    size_t get_length (const std::string &tag) const { return this->_get_item(tag)->length; }
    size_t get_padding(const std::string &tag) const { return this->_get_item(tag)->padding; }

    std::basic_string<uint8_t> dump() const {
        const size_t numbytes = _last.start + _last.length + _last.padding;
        uint8_t *ptr = reinterpret_cast<uint8_t*>(malloc(numbytes));
        assert(ptr);

        memset(ptr, 0, numbytes);
        for (const auto p: _data)
            memcpy(ptr + p.start, p.data, p.length);

        std::basic_string<uint8_t> out(ptr, numbytes);
        free(ptr);

        return std::move(out);
    }

    /*
       Example output:
       tag                 |     start |    length | padding
       ----------------------+-----------+-----------+---------
       main_header         |         0 |      1024 |   0
       main_dawg           |      1024 |     58860 |   0
       main_lookup         |     59884 |     29426 |   2
       main_contents       |     89312 |     16936 |   0
       main_source_str     |    106248 |        27 |   1
       emoji_header        |    106276 |       256 |   0
       emoji_contents      |    106532 |      6198 |   2
       emoji_str           |    112732 |      3892 |   0
       urx_header          |    116624 |       256 |   0
       urx_contents        |    116880 |        30 |   2
       urx_category_str    |    116912 |        31 |   1
       urx_domain_str      |    116944 |        76 |   0
       */
    void print(FILE *fp) {
        fprintf(fp,"    tag                 |     start |    length | padding\n");
        fprintf(fp,"  ----------------------+-----------+-----------+---------\n");
        for (const auto &p: _data)
            p.print(fp);
    }

private:
    struct Item {
        std::string tag;
        const uint8_t *data = nullptr;
        size_t start   = 0;
        size_t length  = 0;
        size_t padding = 0;
        void print(FILE *fp) const {
            fprintf(fp,"    %-20s|%10zu |%10zu |%4zu\n",
                    this->tag.c_str(),
                    this->start,
                    this->length,
                    this->padding
                   );
        }
    };

    const Item *_get_item(const std::string &tag) const {
        size_t i;
        for (i = 0; i < _data.size(); i++)
            if (_data[i].tag == tag)
                break;
        if (i == _data.size()) {
            fprintf(stderr,"Cannot find item for tag: <%s>\n", tag.c_str());
            abort();
        }
        return &_data[i];
    }

    std::vector<Item> _data;
    Item _last;
};

#endif /* end of include guard _BYTES_ALIGNER_H__ */
