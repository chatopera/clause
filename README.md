[![Docker Layers](https://images.microbadger.com/badges/image/chatopera/clause:develop.svg)](https://microbadger.com/images/chatopera/clause:develop "Image layers") [![Docker Version](https://images.microbadger.com/badges/version/chatopera/clause:develop.svg)](https://microbadger.com/images/chatopera/clause:develop "Image version") [![Docker Pulls](https://img.shields.io/docker/pulls/chatopera/clause.svg)](https://hub.docker.com/r/chatopera/clause/) [![Docker Stars](https://img.shields.io/docker/stars/chatopera/clause.svg)](https://hub.docker.com/r/chatopera/clause/) [![Docker Commit](https://images.microbadger.com/badges/commit/chatopera/clause:develop.svg)](https://microbadger.com/images/chatopera/clause:develop "Image CommitID")

# Clause

Chatopera Language Understanding Service，Chatopera 语义理解服务

<p align="center">
  <a href="https://github.com/chatopera/clause" target="_blank">
      <img src="https://static-public.chatopera.com/assets/images/64316956-e4d46d80-cfe8-11e9-8342-ec8a250074bf.png" width="800">
  </a>
</p>

Clause 帮助聊天机器人开发商、开发者快速而低成本的获得开源的语义理解系统。

Clause 是 Chatopera 团队自主研发及使用其他商业友好的开源软件的方式实现的，Clause 为实现聊天机器人提供强大的大脑，包括客服、智能问答和自动流程服务。Clause 利用深度学习，自然语言处理和搜索引擎技术，让机器更加理解人。

利用 Clause 可快速实现聊天机器人服务，通过自然语言的人机交互形式完成数据收集和数据处理。

## 功能

- 支持多机器人管理，每个机器人可创建多个意图(Intent)

- 自定义词典（CustomDict），支持词表形式及正则表达式形式

- 自定义意图（Intent），槽位（Slot）和说法（Utterance）

- 开箱即用的系统词典（人名、地名、组织机构名和时间等）

- 支持聊天机器人调试分支和上线分支

- 支持会话周期管理

- 服务端为微服务，C++实现；客户端使用 RPC 协议连接进行集成，支持多种语言 SDK

- 服务端可做集群，支持大规模高并发访问

**Clause 的服务端使用 C++编写，并且发布为 Docker 镜像；同时提供的客户端集成接口支持多种语言，包括 Java、Python、Node.js 等，请参考下面的内容进一步了解。**

## 快速开始

<p align="center">
  <b>使用 Python 快速实现问答机器人</b><a href="https://github.com/chatopera/clause-py-demo">(链接)</a><br>
  <img src="https://static-public.chatopera.com/assets/images/65892122-54ffc480-e3d8-11e9-8f64-c82f25694df5.gif" width="800">
</p>

## 欢迎使用

- [概述](https://github.com/chatopera/clause/wiki/%E6%A6%82%E8%BF%B0)
- [系统设计与实现](https://github.com/chatopera/clause/wiki/%E7%B3%BB%E7%BB%9F%E8%AE%BE%E8%AE%A1%E4%B8%8E%E5%AE%9E%E7%8E%B0)
- [服务部署](https://github.com/chatopera/clause/wiki/%E6%9C%8D%E5%8A%A1%E9%83%A8%E7%BD%B2)
- [示例程序](https://github.com/chatopera/clause/wiki/%E7%A4%BA%E4%BE%8B%E7%A8%8B%E5%BA%8F)
- [开发环境搭建](https://github.com/chatopera/clause/wiki/%E5%BC%80%E5%8F%91%E7%8E%AF%E5%A2%83%E6%90%AD%E5%BB%BA)
- [系统集成](https://github.com/chatopera/clause/wiki/%E7%B3%BB%E7%BB%9F%E9%9B%86%E6%88%90)
- [API 文档](https://github.com/chatopera/clause/wiki/API%E6%96%87%E6%A1%A3)
- [FAQ](https://github.com/chatopera/clause/wiki/FAQ)

## Chatopera 云服务

Clause 同时也是 [Chatopera 云服务](https://bot.chatopera.com/)的一个基础模块。

[https://bot.chatopera.com/](https://bot.chatopera.com/)

[Chatopera 云服务](https://bot.chatopera.com)是一站式实现聊天机器人的云服务，按接口调用次数计费。Chatopera 云服务是 [Chatopera 机器人平台](https://docs.chatopera.com/products/chatbot-platform/index.html)的软件即服务实例。在云计算基础上，Chatopera 云服务属于**聊天机器人即服务**的云服务。

Chatopera 机器人平台包括知识库、多轮对话、意图识别和语音识别等组件，标准化聊天机器人开发，支持企业 OA 智能问答、HR 智能问答、智能客服和网络营销等场景。企业 IT 部门、业务部门借助 Chatopera 云服务快速让聊天机器人上线！

<details>
<summary>展开查看 Chatopera 云服务的产品截图</summary>
<p>

<p align="center">
  <b>自定义词典</b><br>
  <img src="https://static-public.chatopera.com/assets/images/64530072-da92d600-d33e-11e9-8656-01c26caff4f9.png" width="800">
</p>

<p align="center">
  <b>自定义词条</b><br>
  <img src="https://static-public.chatopera.com/assets/images/64530091-e41c3e00-d33e-11e9-9704-c07a2a02b84e.png" width="800">
</p>

<p align="center">
  <b>创建意图</b><br>
  <img src="https://static-public.chatopera.com/assets/images/64530169-12018280-d33f-11e9-93b4-9db881cf4dd5.png" width="800">
</p>

<p align="center">
  <b>添加说法和槽位</b><br>
  <img src="https://static-public.chatopera.com/assets/images/64530187-20e83500-d33f-11e9-87ec-a0241e3dac4d.png" width="800">
</p>

<p align="center">
  <b>训练模型</b><br>
  <img src="https://static-public.chatopera.com/assets/images/64530235-33626e80-d33f-11e9-8d07-fa3ae417fd5d.png" width="800">
</p>

<p align="center">
  <b>测试对话</b><br>
  <img src="https://static-public.chatopera.com/assets/images/64530253-3d846d00-d33f-11e9-81ea-86e6d47020d8.png" width="800">
</p>

<p align="center">
  <b>机器人画像</b><br>
  <img src="https://static-public.chatopera.com/assets/images/64530312-6442a380-d33f-11e9-869c-85fb6a835a97.png" width="800">
</p>

<p align="center">
  <b>系统集成</b><br>
  <img src="https://static-public.chatopera.com/assets/images/64530281-4ecd7980-d33f-11e9-8def-c53251f30138.png" width="800">
</p>

<p align="center">
  <b>聊天历史</b><br>
  <img src="https://static-public.chatopera.com/assets/images/64530295-5856e180-d33f-11e9-94d4-db50481b2d8e.png" width="800">
</p>

<p align="center">
  <b>立即使用</b><br>
  <a href="https://bot.chatopera.com" target="_blank">
      <img src="https://static-public.chatopera.com/assets/images/64531083-3199aa80-d341-11e9-86cd-3a3ed860b14b.png" width="800">
  </a>
</p>

</p>
</details>

## 主题演讲

| 时间       | 活动                                                                          | 链接                                                                                                                      | 时长   | 概述                                |
| ---------- | ----------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------- | ------ | ----------------------------------- |
| 2019-12-14 | [Microsoft AI Bootscamp(2019)](https://github.com/huan/microsoft-ai-bootcamp) | [回放](https://www.bilibili.com/video/av80153181?p=3)                                                                     | 40mins | 基本使用介绍+支持正则表达式词典     |
| 2019-11-03 | [COSCon '2019 中国开源年会](https://www.bagevent.com/event/5744455)           | [回放](https://v.qq.com/x/page/u3017ka4t6o.html)， [PPT【提取码: 25ni】](https://pan.baidu.com/s/1Z-d6CMtI782TLHRxaklDcw) | 40mins | 基本使用介绍+支持读取文件训练机器人 |
| 2019-09-26 | CSDN 学院直播：深度学习之智能问答机器人实战                                   | [回放](https://edu.csdn.net/huiyiCourse/detail/1068)                                                                      | 60mins | 基本使用介绍                        |

## 用户交流群

<p align="center">
  <b>在 Chatopera 客户群中也包括其他用户，请不要发送敏感信息。讨论与 Chatopera 产品和服务相关的事宜</b><br>
<img src="https://static-public.chatopera.com/assets/images/Chatopera_wecom_customer_group_qr.png" width="600">
</p>

## 媒体报道

- [开源中国：语义理解系统 Clause](https://www.oschina.net/p/clause)

- [我爱自然语言处理：Clause，开源的语义理解服务](http://www.52nlp.cn/clause-oss)

## 特别鸣谢

- [Lexical Analysis of Chinese](https://github.com/baidu/lac): Baidu's open-source lexical analysis tool for Chinese, including word segmentation, part-of-speech tagging & named entity recognition.
- [CRFsuite](https://github.com/chokkan/crfsuite): a fast implementation of Conditional Random Fields (CRFs)
- [Xapian](https://xapian.org/): an Open Source Search Engine Library

## 机器学习 & 自然语言处理入门指南

- Chatopera 出版物[《智能问答与深度学习》](https://item.jd.com/12479014.html)
- [春松客服：开源的智能客服系统](https://github.com/chatopera/cosin)

## 开源许可协议

Copyright (2019-2020) <a href="https://www.chatopera.com/" target="_blank">北京华夏春松科技有限公司</a>

[Apache License Version 2.0](https://github.com/chatopera/clause/blob/master/LICENSE)

[![chatoper banner][co-banner-image]][co-url]

[co-banner-image]: https://static-public.chatopera.com/assets/images/42383104-da925942-8168-11e8-8195-868d5fcec170.png
[co-url]: https://www.chatopera.com
