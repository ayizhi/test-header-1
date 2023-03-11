// SPDX-License-Identifier: Apache-2.0

#ifndef YIJINJING_PAGE_H
#define YIJINJING_PAGE_H

#include <kungfu/yijinjing/journal/common.h>
#include <kungfu/yijinjing/journal/frame.h>

namespace kungfu::yijinjing::journal {

KF_DEFINE_PACK_TYPE(                          //
    page_header, 1, PK(version), PERPETUAL(), //
    (uint32_t, version),                      //
    (uint32_t, page_header_length),           //
    (uint32_t, page_size),                    //
    (uint32_t, frame_header_length),          //
    (uint64_t, last_frame_position)           //
);

class page {
public:
  ~page();

  [[nodiscard]] uint32_t get_page_size() const { return header_->page_size; }

  [[nodiscard]] data::location_ptr get_location() const { return location_; }

  [[nodiscard]] uint32_t get_dest_id() const { return dest_id_; }

  [[nodiscard]] uint32_t get_page_id() const { return page_id_; }

  [[nodiscard]] int64_t begin_time() const { return reinterpret_cast<frame_header *>(first_frame_address())->gen_time; }

  [[nodiscard]] int64_t end_time() const { return reinterpret_cast<frame_header *>(last_frame_address())->gen_time; }

  [[nodiscard]] uintptr_t address() const { return reinterpret_cast<uintptr_t>(header_); }

  [[nodiscard]] uintptr_t address_border() const { return address() + header_->page_size - sizeof(frame_header); }

  [[nodiscard]] uintptr_t first_frame_address() const { return address() + header_->page_header_length; }

  [[nodiscard]] uintptr_t last_frame_address() const { return address() + header_->last_frame_position; }

  [[nodiscard]] bool is_full() const {
    return last_frame_address() + reinterpret_cast<frame_header *>(last_frame_address())->length > address_border();
  }

  static page_ptr load(const data::location_ptr &location, uint32_t dest_id, uint32_t page_id, bool is_writing,
                       bool lazy);

  static std::string get_page_path(const data::location_ptr &location, uint32_t dest_id, uint32_t page_id);

  static uint32_t find_page_id(const data::location_ptr &location, uint32_t dest_id, int64_t time);

private:
  const data::location_ptr location_;
  const uint32_t dest_id_;
  const uint32_t page_id_;
  const bool lazy_;
  const size_t size_;
  const page_header *header_;

  page(data::location_ptr location, uint32_t dest_id, uint32_t page_id, size_t size, bool lazy, uintptr_t address);

  /**
   * update page header when new frame added
   */
  void set_last_frame_position(uint64_t position);

  friend class journal;

  friend class writer;

  friend class reader;
};

inline static uint32_t find_page_size(const data::location_ptr &location, uint32_t dest_id) {
  if (location->category == longfist::enums::category::MD && dest_id != 1) {
    return 128 * MB;
  }
  if ((location->category == longfist::enums::category::TD ||
       location->category == longfist::enums::category::STRATEGY) &&
      dest_id != 0) {
    return 16 * MB;
  }
  return MB;
}
} // namespace kungfu::yijinjing::journal

#endif // YIJINJING_PAGE_H
