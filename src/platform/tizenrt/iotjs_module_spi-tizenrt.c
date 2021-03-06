/* Copyright 2017-present Samsung Electronics Co., Ltd. and other contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if defined(__TIZENRT__)

#include <iotbus_spi.h>
#include <tinyara/config.h>

#if !defined(CONFIG_SPI)
#error "\n\nTizenRT CONFIG_SPI configuration flag required for SPI module\n\n"
#elif !defined(CONFIG_SPI_EXCHANGE)
#error "\n\nTizenRT CONFIG_SPI_EXCHANGE flag required for SPI module\n\n"
#endif

#include <iotbus_error.h>
#include <iotbus_spi.h>

#include "modules/iotjs_module_spi.h"


struct _iotjs_spi_module_platform_t {
  unsigned int bus;
  iotbus_spi_context_h hSpi;
};

static bool iotjs_spi_open(iotjs_spi_t* spi) {
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_spi_t, spi);

  struct iotbus_spi_config_s cfg = {.bits_per_word = _this->bits_per_word,
                                    .chip_select =
                                        _this->chip_select == kSpiCsNone ? 0
                                                                         : 1,
                                    .frequency = _this->max_speed };

  switch (_this->mode) {
    case kSpiMode_0:
      cfg.mode = IOTBUS_SPI_MODE0;
      break;
    case kSpiMode_1:
      cfg.mode = IOTBUS_SPI_MODE1;
      break;
    case kSpiMode_2:
      cfg.mode = IOTBUS_SPI_MODE2;
      break;
    case kSpiMode_3:
      cfg.mode = IOTBUS_SPI_MODE3;
      break;
    default:
      cfg.mode = IOTBUS_SPI_MODE0;
  }

  _this->platform->hSpi = iotbus_spi_open(_this->platform->bus, &cfg);
  if (_this->platform->hSpi == NULL) {
    return false;
  }

  DDLOG(
      "SPI Options \n mode: %d\n chipSelect: %d\n bitOrder: %d\n "
      "maxSpeed: %d\n bitPerWord: %d\n loopback: %d",
      _this->mode, _this->chip_select, _this->bit_order, _this->max_speed,
      _this->bits_per_word, _this->loopback);

  return true;
}


bool iotjs_spi_transfer(iotjs_spi_t* spi) {
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_spi_t, spi);

  int err = iotbus_spi_transfer_buf(_this->platform->hSpi,
                                    (unsigned char*)_this->tx_buf_data,
                                    (unsigned char*)_this->rx_buf_data,
                                    _this->buf_len);
  if (err != 0) {
    DDLOG("%s - transfer failed: %d", __func__, err);
    return false;
  }

  return true;
}


bool iotjs_spi_close(iotjs_spi_t* spi) {
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_spi_t, spi);

  if (_this->platform->hSpi != NULL) {
    int err = iotbus_spi_close(_this->platform->hSpi);
    if (err != 0) {
      DDLOG("%s - close failed: %d", __func__, err);
      return false;
    }
    _this->platform->hSpi = NULL;
  }

  return true;
}


void iotjs_spi_open_worker(uv_work_t* work_req) {
  SPI_WORKER_INIT;
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_spi_t, spi);

  if (_this != 0) { // use of _this to prevent compiler warnings
  }

  if (!iotjs_spi_open(spi)) {
    DLOG("%s - SPI open failed %d", __func__, _this->platform->bus);
    req_data->result = false;
    return;
  }

  req_data->result = true;
}

void iotjs_spi_platform_create(iotjs_spi_t_impl_t* _this) {
}

void iotjs_spi_platform_destroy(iotjs_spi_t_impl_t* _this) {
}

void iotjs_spi_platform_set_cofiguration(iotjs_spi_t_impl_t* _this,
                                         const iotjs_jval_t* joptions) {
  iotjs_jval_t jbus = iotjs_jval_get_property(joptions, IOTJS_MAGIC_STRING_BUS);
  _this->platform->bus = iotjs_jval_as_number(&jbus);
  iotjs_jval_destroy(&jbus);
}
#endif // __TIZENRT__
