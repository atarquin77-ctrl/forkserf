/*
 * data-source-overlay.h - Custom-first resource overlay for Settlers 1.0
 *
 * This file is part of the Settlers 1.0 experimental branch of Forkserf.
 */

#ifndef SRC_DATA_SOURCE_OVERLAY_H_
#define SRC_DATA_SOURCE_OVERLAY_H_

#include <string>
#include <tuple>
#include <utility>

#include "src/data.h"

class DataSourceOverlay : public Data::Source {
 public:
  DataSourceOverlay(Data::PSource custom, Data::PSource fallback)
      : custom_(std::move(custom)), fallback_(std::move(fallback)) {}

  std::string get_name() const override {
    return "Custom-first overlay";
  }

  std::string get_path() const override {
    if (custom_) {
      return custom_->get_path();
    }
    return fallback_ ? fallback_->get_path() : std::string();
  }

  bool is_loaded() const override {
    return fallback_ && fallback_->is_loaded();
  }

  unsigned int get_scale() const override {
    if (fallback_) {
      return fallback_->get_scale();
    }
    return custom_ ? custom_->get_scale() : 1;
  }

  unsigned int get_bpp() const override {
    if (fallback_) {
      return fallback_->get_bpp();
    }
    return custom_ ? custom_->get_bpp() : 0;
  }

  bool check() override {
    return is_loaded();
  }

  bool load() override {
    return is_loaded();
  }

  Data::PSprite get_sprite(Data::Resource res, size_t index,
                           const Data::Sprite::Color &color,
                           int mutate = 0) override {
    if (custom_) {
      Data::PSprite sprite = custom_->get_sprite(res, index, color, mutate);
      if (sprite) {
        return sprite;
      }
    }

    if (fallback_) {
      return fallback_->get_sprite(res, index, color, mutate);
    }

    return nullptr;
  }

  Data::MaskImage get_sprite_parts(Data::Resource res, size_t index,
                                   int mutate = 0) override {
    if (custom_) {
      Data::MaskImage parts = custom_->get_sprite_parts(res, index, mutate);
      if (std::get<0>(parts) || std::get<1>(parts)) {
        return parts;
      }
    }

    if (fallback_) {
      return fallback_->get_sprite_parts(res, index, mutate);
    }

    return std::make_tuple(nullptr, nullptr);
  }

  size_t get_animation_phase_count(size_t animation) override {
    return fallback_ ? fallback_->get_animation_phase_count(animation) : 0;
  }

  Data::Animation get_animation(size_t animation, size_t phase) override {
    if (fallback_) {
      return fallback_->get_animation(animation, phase);
    }

    return {0, 0, 0};
  }

  PBuffer get_sound(size_t index) override {
    return fallback_ ? fallback_->get_sound(index) : nullptr;
  }

  Data::MusicFormat get_music_format() override {
    return fallback_ ? fallback_->get_music_format() : Data::MusicFormatNone;
  }

  PBuffer get_music(size_t index) override {
    return fallback_ ? fallback_->get_music(index) : nullptr;
  }

  bool check_file(const std::string &path) override {
    if (custom_ && custom_->check_file(path)) {
      return true;
    }
    return fallback_ ? fallback_->check_file(path) : false;
  }

 private:
  Data::PSource custom_;
  Data::PSource fallback_;
};

#endif  // SRC_DATA_SOURCE_OVERLAY_H_
