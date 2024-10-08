﻿/*
 * Copyright (c) 2016-present The ZLMediaKit project authors. All Rights Reserved.
 *
 * This file is part of ZLMediaKit(https://github.com/ZLMediaKit/ZLMediaKit).
 *
 * Use of this source code is governed by MIT-like license that can be found in the
 * LICENSE file in the root of the source tree. All contributing project authors
 * may be found in the AUTHORS file in the root of the source tree.
 */

#ifndef ZLMEDIAKIT_TSMEDIASOURCE_H
#define ZLMEDIAKIT_TSMEDIASOURCE_H

#include "Common/MediaSource.h"
#include "Common/PacketCache.h"
#include "Util/RingBuffer.h"

#define TS_GOP_SIZE 512

namespace mediakit {

// TS直播数据包  [AUTO-TRANSLATED:02fb2e8e]
// TS Live Data Packet
class TSPacket : public toolkit::BufferOffset<toolkit::Buffer::Ptr>{
public:
    using Ptr = std::shared_ptr<TSPacket>;

    template<typename ...ARGS>
    TSPacket(ARGS && ...args) : BufferOffset<Buffer::Ptr>(std::forward<ARGS>(args)...) {};

public:
    uint64_t time_stamp = 0;
};

// TS直播源  [AUTO-TRANSLATED:0d25ead6]
// TS Live Source
class TSMediaSource final : public MediaSource, public toolkit::RingDelegate<TSPacket::Ptr>, private PacketCache<TSPacket>{
public:
    using Ptr = std::shared_ptr<TSMediaSource>;
    using RingDataType = std::shared_ptr<toolkit::List<TSPacket::Ptr> >;
    using RingType = toolkit::RingBuffer<RingDataType>;

    TSMediaSource(const MediaTuple& tuple, int ring_size = TS_GOP_SIZE): MediaSource(TS_SCHEMA, tuple), _ring_size(ring_size) {}

    ~TSMediaSource() override {
        try {
            flush();
        } catch (std::exception &ex) {
            WarnL << ex.what();
        }
    }

    /**
     * 获取媒体源的环形缓冲
     * Get the circular buffer of the media source
     
     * [AUTO-TRANSLATED:91a762bc]
     */
    const RingType::Ptr &getRing() const {
        return _ring;
    }

    void getPlayerList(const std::function<void(const std::list<toolkit::Any> &info_list)> &cb,
                       const std::function<toolkit::Any(toolkit::Any &&info)> &on_change) override {
        _ring->getInfoList(cb, on_change);
    }

    /**
     * 获取播放器个数
     * Get the number of players
     
     * [AUTO-TRANSLATED:a451c846]
     */
    int readerCount() override {
        return _ring ? _ring->readerCount() : 0;
    }

    /**
     * 输入TS包
     * @param packet TS包
     * @param key 是否为关键帧第一个包
     * Input TS packet
     * @param packet TS packet
     * @param key Whether it is the first packet of the key frame
     
     * [AUTO-TRANSLATED:cd773549]
     */
    void onWrite(TSPacket::Ptr packet, bool key) override {
        _speed[TrackVideo] += packet->size();
        if (!_ring) {
            createRing();
        }
        if (key) {
            _have_video = true;
        }
        auto stamp = packet->time_stamp;
        PacketCache<TSPacket>::inputPacket(stamp, true, std::move(packet), key);
    }

    /**
     * 情况GOP缓存
     * Clear GOP cache
     
     * [AUTO-TRANSLATED:d863f8c9]
     */
    void clearCache() override {
        PacketCache<TSPacket>::clearCache();
        _ring->clearCache();
    }

private:
    void createRing(){
        std::weak_ptr<TSMediaSource> weak_self = std::static_pointer_cast<TSMediaSource>(shared_from_this());
        _ring = std::make_shared<RingType>(_ring_size, [weak_self](int size) {
            auto strong_self = weak_self.lock();
            if (!strong_self) {
                return;
            }
            strong_self->onReaderChanged(size);
        });
        // 注册媒体源  [AUTO-TRANSLATED:b87b5ac4]
        // Register media source
        regist();
    }

    /**
     * 合并写回调
     * @param packet_list 合并写缓存列队
     * @param key_pos 是否包含关键帧
     * Merge write callback
     * @param packet_list Merge write cache queue
     * @param key_pos Whether it contains a key frame
     
     * [AUTO-TRANSLATED:6e93913e]
     */
    void onFlush(std::shared_ptr<toolkit::List<TSPacket::Ptr> > packet_list, bool key_pos) override {
        // 如果不存在视频，那么就没有存在GOP缓存的意义，所以确保一直清空GOP缓存  [AUTO-TRANSLATED:66208f94]
        // If there is no video, then there is no meaning to the existence of GOP cache, so make sure to clear the GOP cache all the time
        _ring->write(std::move(packet_list), _have_video ? key_pos : true);
    }

private:
    bool _have_video = false;
    int _ring_size;
    RingType::Ptr _ring;
};


}//namespace mediakit
#endif //ZLMEDIAKIT_TSMEDIASOURCE_H
