# global-illumination
`Global Illumination = “virtual photography”
Given a scene description that specifies the location of surfaces in a scene, the location of lights, and the location of a camera, take a virtual “photograph” of that scene.`

本项目作为课程设计内容实现了一个全局光照的渲染器，并结合代码讲解路径追踪算法的基本原理。

## 路径追踪算法的基本原理
路径追踪和光线追踪的基本思路是一样的，只不过我们需要在采样路径上多加考量，在项目中路径追踪主要是指蒙特卡洛路径追踪（Monte Carlo Path Tracing）
### 渲染方程
我们有如下渲染方程：
![rendering-equation](./graph/rendering-equation.bmp)
**方程含义**

1.$L(P \rightarrow D_v)$：计算从点$P$沿观察方向$D_v$的光亮度。\
2.$L_e(P \rightarrow D_v)$：如果$P$是光源，则直接贡献自发光。\
3.积分项：计算所有可能的入射光$𝐷_𝑖$对$𝑃$的贡献：\
- BRDF($F_s$): 决定多少入射光被反射到$Dv$.
- $(cosθ)$: 入射角度越大，贡献越小（类似斜照光变暗）。
- 递归的$L(Y_i\rightarrow -D_i)$: 入射光本身可能是其他点的反射光（全局光照）。
### 蒙特卡洛积分
可以从渲染方程中看出，最关键的部分是在半球面上积分的部分，入射方向可以有无穷多个，并且该积分很难找到解析解，我们需要一种方法能够用有限次的计算近似替代无限的积分结果，这就需要用到统计学的知识。\
**蒙特卡洛积分公式**
$$
\int_a^b f(x)dx \approx \frac{1}{N} \sum_{k=1}^N \frac{f(X_k)}{p(X_k)}, \quad X_k \sim p(x)
$$
其中：
- $p(x)$是采样概率密度函数(PDF)。
- $X_k$是从$p(x)$中抽取的随机样本。

将蒙特卡洛方法代入渲染方程的积分项：
$$
\int_{\Omega} F_s(D_v, D_i) |\cos \theta| L(Y_i \rightarrow -D_i) \, dD_i \approx \frac{1}{N} \sum_{k=1}^N \frac{F_s(D_v, D_i^{(k)}) |\cos \theta^{(k)}| L(Y_i^{(k)} \rightarrow -D_i^{(k)})}{p(D_i^{(k)})}
$$
如果在半球面上均匀采样$N$次立体角$D_i^{(k)}$,我们有$$D_i^{(k)} \sim p(D_i^{(k)}) = \frac{1}{2\pi}$$
### 路径追踪的实现

跟据上面公式我们得到路径追踪的代码。
```
shade(p,Dv)
    Randomly choose N directons Di ~ pdf
    Lo = 0.0
    for each Di
        Trace a ray r(p,Di)
        //光源的光
        If ray r hit the light
            Lo +=(1/N) * L_i * f_r * cosine / pdf(Di)
        //碰到其他物体反射的光
        Else If ray r hit an object at q
        Lo +=(1/N) * shade(y,-wi) * L_i * f_r * cosine / pdf(Di)
```
上面代码有两个问题：\
1.如果每一次递归都打出 $N$ 条光线，那么递归 $n$ 次之后，光线一共膨胀成了$n^{N}$条，如果$N>1$，那么其呈指数增长趋势，这种计算量是爆炸的，唯一的解决方法是取$N = 1$，**即每次只取一条光线。**\
但是这就有问题了，本来是一个采样统计的方法，结果现在只取一条光线，那么结果肯定是不准确的。所以在这里就需要**在一个像素内进行多条路径采样**，将每条路径得到的结果按照一定方式取平均，得到最终该像素的亮度。\
算法如下，多了生成光线的过程，在一个像素中随机取$N$条路径，再代入$shade(p,Di)$计算，将$N$条路径得到的结果取平均作为该像素最终的值。
```
ray_generation(camPos, pixel)
    Uniformly choose N sample positions within the pixel
    pixel_radiance = 0.0
    For each sample in the pixel
        Shoot a ray r(camPos, cam_to_sample)
        If ray r hit the scene at p
            pixel_radiance += 1/N * shade(p,sample_to_cam)
    return pixel_radiance
```


2.在前面的 $shade(p,Dv)$中，我们并没有设置递归什么时候结束。虽然在现实中光线确实可以通过无限次弹射，但是在程序中我们需要考虑为其设置停止条件，这里用到的方法是俄罗斯轮盘赌，同样也是一个概率方法，它是说在每次递归中，生成一个随机数，如果该随机数小于我们设定的概率值，那么递归可以继续进行，否则就可以结束递归返回了。

最终我们的代码如下：
```
shade(p,Dv)
    Manually specify a probability P-RP
    Randomly select ksi in a uniform dist. in [0, 1]
    If(ksi > P_RR) return 0.0

    Randomly choose ONE directons Di ~ pdf
    Trace a ray r(p,Di)
    If ray r hit the light
        return L_i * f_r * cosin / pdf(Di) / P_RR
    Else If ray r hit an object at q
        return shade(y,-Di) * f_r * cosin / pdf(Di) / P_RR
```
## 代码解读
### 数据结构
#### Vec3d
首先定义三维向量 Vec3d，可以用来表示位置、方向、颜色等，它包含 x、y、z 三个分量，重载算子 +、-、*，分别用于向量相加、向量相减、向量与标量相乘，定义 mult、norm、dot、cross 方法，分别用于向量各项相乘、归一化、点乘、叉乘。
```
struct Vec3d
{
    double x, y, z; 
    Vec3d(double x_ = 0, double y_ = 0, double z_ = 0)
    {
        x = x_;
        y = y_;
        z = z_;
    }
    Vec3d operator+(const Vec3d& b) const { return Vec3d(x + b.x, y + b.y, z + b.z); }
    Vec3d operator-(const Vec3d& b) const { return Vec3d(x - b.x, y - b.y, z - b.z); }
    Vec3d operator*(double b) const { return Vec3d(x * b, y * b, z * b); }
    Vec3d mult(const Vec3d& b) const { return Vec3d(x * b.x, y * b.y, z * b.z); }
    Vec3d& norm() { return *this = *this * (1 / sqrt(x * x + y * y + z * z)); }
    double dot(const Vec3d& b) const { return x * b.x + y * b.y + z * b.z; }
    Vec3d cross(Vec3d& b) { return Vec3d(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x); }
};
```
#### Ray
定义光线结构 Ray，包含起点 o 与方向 d。
```
struct Ray
{
    Vec3d o, d;
    Ray(Vec3d o_, Vec3d d_) : o(o_), d(d_) {}
};
```
#### Refl_t
```
enum Refl_t
{
    DIFF,
    SPEC,
    REFR
};
```
#### Sphere
由于球体与光线求交最简单，这里只定义了球体类型 Sphere，包含半径 rad、位置 p、自发光 e、自身颜色 c、反射类型 refl。同时包含一个判断光线是否与自己相交的方法 intersect，如果相交返回交点与光线原点的距离，否则返回 0。
```
struct Sphere
{
    double rad;  
    Vec3d p, e, c; 
    Refl_t refl; 
    Sphere(double rad_, Vec3d p_, Vec3d e_, Vec3d c_, Refl_t refl_) : rad(rad_), p(p_), e(e_), c(c_), refl(refl_) {}
    double intersect(const Ray& r) const
    {                   
        Vec3d op = p - r.o; 
        double t, eps = 1e-4, b = op.dot(r.d), det = b * b - op.dot(op) + rad * rad;
        if (det < 0)
            return 0;
        else
            det = sqrt(det);
        return (t = b - det) > eps ? t : ((t = b + det) > eps ? t : 0);
    }
};
```
下面是判断光线与球体是否相交的方法
![sphere Intersection](./graph/spher%20Intersection.bmp)
