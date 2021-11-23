# 微引擎
最最基本的引擎feature：
- 平行光
- 点光源
- 阴影
- AABB相交检测
- 基本图元Cube，Plane
- 蒙皮Mesh和非Mesh
- 天空盒
- 仅支持fbx的模型和动画导入

## 依赖
GLM
GLFW
OpenGL ES API
FBX API

## 结构
SceneManager - 节点管理，主要分为4类节点，天空盒/相机/光源/可绘制节点，控制PlayLoop，调用Render绘制，实现事件监听用于改变场景的画面
GraphicsRender - 图形渲染器，实现为OpenGLGraphicsRender，将节点的绘制变成gl绘制命令，控制场景渲染
EventListener - 事件监听，游戏需要和用户交互，场景管理器需要注册监听事件

游戏应用利用场景管理器创建游戏场景，并实现事件监听接口用于处理游戏交互逻辑，场景管理器会调用图形渲染器，逐帧渲染游戏画面，这样就能利用微引擎打造一个简单的游戏。