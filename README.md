[![Docker Layers](https://images.microbadger.com/badges/image/chatopera/clause:develop.svg)](https://microbadger.com/images/chatopera/clause:develop "Image layers") [![Docker Version](https://images.microbadger.com/badges/version/chatopera/clause:develop.svg)](https://microbadger.com/images/chatopera/clause:develop "Image version") [![Docker Pulls](https://img.shields.io/docker/pulls/chatopera/clause.svg)](https://hub.docker.com/r/chatopera/clause/) [![Docker Stars](https://img.shields.io/docker/stars/chatopera/clause.svg)](https://hub.docker.com/r/chatopera/clause/) [![Docker Commit](https://images.microbadger.com/badges/commit/chatopera/clause:develop.svg)](https://microbadger.com/images/chatopera/clause:develop "Image CommitID")

<p align="center">
  <b>Clause项目QQ交流群：809987971， <a href="https://jq.qq.com/?_wv=1027&k=5JpEvBZ" target="_blank">点击链接加入群聊</a></b><br>
  <img src="https://user-images.githubusercontent.com/3538629/64315364-6a095380-cfe4-11e9-8bf6-f15ce6e26e0a.png" width="200">
</p>

# Clause

Chatopera Language Understanding Service，Chatopera 语义理解服务

<p align="center">
  <a href="https://github.com/chatopera/clause" target="_blank">
      <img src="https://user-images.githubusercontent.com/3538629/64316956-e4d46d80-cfe8-11e9-8342-ec8a250074bf.png" width="800">
  </a>
</p>

Clause 是帮助中小型企业快速而低成本的获得好用的语义理解服务的系统。

Clause 是 Chatopera 团队自主研发及使用其他商业友好的开源软件的方式实现的，Clause 为实现企业聊天机器人提供强大的大脑，包括客服、智能问答和自动流程服务。Clause 利用深度学习，自然语言处理和搜索引擎技术，让机器更加理解人。

**Clause 的服务端使用 C++编写，并且发布为 Docker 镜像；同时提供的客户端集成接口支持多种语言，包括 Java、Python、Node.js 等，请参考下面的内容进一步了解。**

## 快速开始

<p align="center">
  <b>使用 Python 快速实现问答机器人</b><a href="https://github.com/chatopera/clause-py-demo">(链接)</a><br>
  <img src="https://user-images.githubusercontent.com/3538629/65892122-54ffc480-e3d8-11e9-8f64-c82f25694df5.gif" width="800">
</p>

## 欢迎使用

- [概述](https://github.com/chatopera/clause/wiki/%E6%A6%82%E8%BF%B0)
- [系统设计与实现](https://github.com/chatopera/clause/wiki/%E7%B3%BB%E7%BB%9F%E8%AE%BE%E8%AE%A1%E4%B8%8E%E5%AE%9E%E7%8E%B0)
- [服务部署](https://github.com/chatopera/clause/wiki/%E6%9C%8D%E5%8A%A1%E9%83%A8%E7%BD%B2)
- [示例程序](https://github.com/chatopera/clause/wiki/%E7%A4%BA%E4%BE%8B%E7%A8%8B%E5%BA%8F)
- [开发环境搭建](https://github.com/chatopera/clause/wiki/%E5%BC%80%E5%8F%91%E7%8E%AF%E5%A2%83%E6%90%AD%E5%BB%BA)
- [系统集成](https://github.com/chatopera/clause/wiki/%E7%B3%BB%E7%BB%9F%E9%9B%86%E6%88%90)
- [API 文档](https://chatopera.github.io/clause)
- [FAQ](https://github.com/chatopera/clause/wiki/FAQ)

## Chatopera 云服务

Clause 同时也是 [Chatopera 云服务](https://bot.chatopera.com/)的一个基础模块。[Chatopera 云服务](https://bot.chatopera.com/)提供基于搜索、规则和机器学习的对话解决方案，可用于企业定制满足自身业务需求的聊天机器人，同时 [Chatopera 云服务](https://bot.chatopera.com/)通过计费、支付和发票等方法降低了企业获取这样一个解决方案的成本，是企业更快的专注于设计对话、调试聊天机器人，更快的投入使用。 **以下图片来自于 Chatopera 云服务的意图识别模块，其底层 API 使用 Clause 服务。**

<p align="center">
  <b>自定义词典</b><br>
  <img src="https://user-images.githubusercontent.com/3538629/64530072-da92d600-d33e-11e9-8656-01c26caff4f9.png" width="800">
</p>

<p align="center">
  <b>自定义词条</b><br>
  <img src="https://user-images.githubusercontent.com/3538629/64530091-e41c3e00-d33e-11e9-9704-c07a2a02b84e.png" width="800">
</p>

<p align="center">
  <b>创建意图</b><br>
  <img src="https://user-images.githubusercontent.com/3538629/64530169-12018280-d33f-11e9-93b4-9db881cf4dd5.png" width="800">
</p>

<p align="center">
  <b>添加说法和槽位</b><br>
  <img src="https://user-images.githubusercontent.com/3538629/64530187-20e83500-d33f-11e9-87ec-a0241e3dac4d.png" width="800">
</p>

<p align="center">
  <b>训练模型</b><br>
  <img src="https://user-images.githubusercontent.com/3538629/64530235-33626e80-d33f-11e9-8d07-fa3ae417fd5d.png" width="800">
</p>

<p align="center">
  <b>测试对话</b><br>
  <img src="https://user-images.githubusercontent.com/3538629/64530253-3d846d00-d33f-11e9-81ea-86e6d47020d8.png" width="800">
</p>

<p align="center">
  <b>机器人画像</b><br>
  <img src="https://user-images.githubusercontent.com/3538629/64530312-6442a380-d33f-11e9-869c-85fb6a835a97.png" width="800">
</p>

<p align="center">
  <b>系统集成</b><br>
  <img src="https://user-images.githubusercontent.com/3538629/64530281-4ecd7980-d33f-11e9-8def-c53251f30138.png" width="800">
</p>

<p align="center">
  <b>聊天历史</b><br>
  <img src="https://user-images.githubusercontent.com/3538629/64530295-5856e180-d33f-11e9-94d4-db50481b2d8e.png" width="800">
</p>

## 立即使用

Chatopera 云服务是面向企业聊天机器人构建的一站式解决方案。

<p align="center">
  <b>Chatopera云服务</b><br>
  <a href="https://bot.chatopera.com" target="_blank">
      <img src="https://user-images.githubusercontent.com/3538629/64531083-3199aa80-d341-11e9-86cd-3a3ed860b14b.png" width="800">
  </a>
</p>

## 媒体报道

- [CSDN 学院直播：深度学习之智能问答机器人实战(含视频回放)](https://edu.csdn.net/huiyiCourse/detail/1068)
- [我爱自然语言处理：Clause，开源的语义理解服务](http://www.52nlp.cn/clause-oss)
- [开源中国：语义理解系统 Clause](https://www.oschina.net/p/clause)

## 特别鸣谢

- [Lexical Analysis of Chinese](https://github.com/baidu/lac): Baidu's open-source lexical analysis tool for Chinese, including word segmentation, part-of-speech tagging & named entity recognition.
- [CRFsuite](https://github.com/chokkan/crfsuite): a fast implementation of Conditional Random Fields (CRFs)
- [Xapian](https://xapian.org/): an Open Source Search Engine Library

## 其它

- Chatopera 出版物[《智能问答与深度学习》](https://item.jd.com/12479014.html)
- [春松客服：开源的智能客服系统](https://github.com/chatopera/cosin)

## 开源许可协议

Copyright (2019) <a href="https://www.chatopera.com/" target="_blank">北京华夏春松科技有限公司</a>

[Apache License Version 2.0](https://github.com/chatopera/clause/blob/master/LICENSE)

[![chatoper banner][co-banner-image]][co-url]

[co-banner-image]: https://user-images.githubusercontent.com/3538629/42383104-da925942-8168-11e8-8195-868d5fcec170.png
[co-url]: https://www.chatopera.com
