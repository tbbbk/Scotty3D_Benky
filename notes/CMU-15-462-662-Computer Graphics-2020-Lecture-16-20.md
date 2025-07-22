[TOC]

# Lecture 16: The Rendering Equation

**Recap: What is radiance?**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428151000006.png" alt="image-20250428151000006" style="zoom:50%;" />

---

**The Rendering Equation**
$$
L_o(p, \omega_o) = L_e(p, \omega_o) + \int_{\Omega} L_i(p, \omega_i) \cdot f_r(p, \omega_i, \omega_o) \cdot \cos\theta_i \, d\omega_i
$$
<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428154254301.png" alt="image-20250428154254301" style="zoom:75%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428152153197.png" alt="image-20250428152153197" style="zoom:50%;" />

**Recursive Raytracing**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428153956535.png" alt="image-20250428153956535" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428154144862.png" alt="image-20250428154144862" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428154313243.png" alt="image-20250428154313243" style="zoom:50%;" />

---

**Reflection models**

Different surface texture has the different reflection for incoming light.

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428154358606.png" alt="image-20250428154358606" style="zoom:50%;" />

Some basic reflection models:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428154410459.png" alt="image-20250428154410459" style="zoom:50%;" />

> Keenan: @all Yeah, good point. I don't think that's a suitable description of what's going on with the moon. (To be candid: I didn't make this slide! ;-))
>
> A better explanation is that the surface of the moon is very rough, with lots of little bumps facing in every direction. Even if the reflection off each little bump looks perfectly diffuse, in the aggregate (i.e., from a great distance) the reflection will look different.
>
> One common model for more realistic reflections off of "rough" diffuse surfaces is the [Oren-Nayar](https://en.wikipedia.org/wiki/Oren–Nayar_reflectance_model) model. Maybe you can implement this in Scotty3D---and render the moon! :-D

---

**Model of Scattering**

The $f_r$ function in the rendering equation, determining how the light transpose along the ray:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428154828152.png" alt="image-20250428154828152" style="zoom:50%;" />

---

**The BRDF**

Bidirectional reflectance distribution function (the $f_r$ function in the rendering equation)

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428155016318.png" alt="image-20250428155016318" style="zoom:50%;" />

> Some basic BRDF models:
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428160136556.png" alt="image-20250428160136556" style="zoom:50%;" />
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428160202189.png" alt="image-20250428160202189" style="zoom:50%;" />

---

**The Radiometric description of BRDF**

For given change in the incident irradiance, how much does the exitant radiance change?

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428155355664.png" alt="image-20250428155355664" style="zoom:50%;" />

---

**Snell's Law**

折射

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428160300180.png" alt="image-20250428160300180" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428160322570.png" alt="image-20250428160322570" style="zoom:50%;" />

---

**Fresnel reflection**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428160354655.png" alt="image-20250428160354655" style="zoom:50%;" />

> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428160412556.png" alt="image-20250428160412556" style="zoom:50%;" />

---

**Anisotropic reflection**

各向异性反射

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428160447266.png" alt="image-20250428160447266" style="zoom:50%;" />

---

**Subsurface scattering**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428160532204.png" alt="image-20250428160532204" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428160602966.png" alt="image-20250428160602966" style="zoom:50%;" />

---

**Summary**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428160712105.png" alt="image-20250428160712105" style="zoom:50%;" />

> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428160833503.png" alt="image-20250428160833503" style="zoom:50%;" />

Next lecture will contain the the content about Monte Carlo

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428160910719.png" alt="image-20250428160910719" style="zoom:50%;" />

# Lecture 17: Numerical Integration

数值积分，有的时候直接求积分是没有解析解的，所以需要用数值方法近似

> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512205740692.png" alt="image-20250512205740692" style="zoom:50%;" />
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512205808232.png" alt="image-20250512205808232" style="zoom:50%;" />

**Gauss Quadrature**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512205940202.png" alt="image-20250512205940202" style="zoom:50%;" />

> 在 **Gauss-Legendre Quadrature** 中：
>
> - 使用 n 个点（节点），可以**精确积分所有次数小于等于 2n - 1 的多项式**。
> - 所以，当 n=2n = 2（两个点）时，它可以精确计算**所有三次及以下多项式**的定积分。
>
> ------
>
> ### 举个简单的例子：
>
> 设你有一个函数：
> $$
> f(x) = 3x^3 + 2x^2 + x + 1
> $$
> 这个函数是一个三次多项式。
>
> 使用 **2点 Gauss-Legendre 积分**：
> $$
> \int_{-1}^{1} f(x) dx \approx f\left(-\frac{1}{\sqrt{3}}\right) + f\left(\frac{1}{\sqrt{3}}\right)
> $$
> （因为权重都是1）
>
> 你会发现这个近似值和解析积分结果是**完全一样的**。
>
> ------
>
> ### 为什么这么强？
>
> 这是因为 Gauss Quadrature 的采样点（Legendre 多项式的零点）和权重是**精心设计的**，使得：
> $$
> \int_{-1}^{1} p(x) dx = \sum_{i=1}^{n} w_i p(x_i)
> $$
> 对所有$\deg(p) \leq 2n - 1$ 的多项式p(x) 成立。
>
> ------
>
> 这也是为什么在图形学中处理光照、BRDF积分、形状函数等问题时，高斯求积法能用极少的采样点就获得非常准确的结果。

---

**Piece affine function**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512211021338.png" alt="image-20250512211021338" style="zoom:50%;" />

---

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512211056108.png" alt="image-20250512211056108" style="zoom:50%;" />

---

**Trapezoid rule**

Approximate integral by pretending the function is piecewise affine

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512211227603.png" alt="image-20250512211227603" style="zoom:50%;" />

n 越大（h 越小），整体的cost就越大，但是误差就会变小

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512211314261.png" alt="image-20250512211314261" style="zoom:50%;" />

> Why the error is $O(h^2)$:
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512211736286.png" alt="image-20250512211736286" style="zoom:67%;" />

---

**Integration in 2D**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512211827870.png" alt="image-20250512211827870" style="zoom:50%;" />

The error is still $O(h^2)$, but the cost goes up to $O(n^2)$ 

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512212453328.png" alt="image-20250512212453328" style="zoom:50%;" />

---

**Curse of dimensionality**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512212500045.png" alt="image-20250512212500045" style="zoom:50%;" />

---

**Monte Carlo Integration**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512215928108.png" alt="image-20250512215928108" style="zoom:50%;" />

> 有个笔误，右下角是$n^{-1/2}$

---

**Sampling continuous random variables using the inversion method**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512212840751.png" alt="image-20250512212840751" style="zoom:50%;" />

> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512212924085.png" alt="image-20250512212924085" style="zoom:50%;" />

---

**How do we uniformly sample the unit circle?**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512213524399.png" alt="image-20250512213524399" style="zoom:50%;" />

First try:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512213547388.png" alt="image-20250512213547388" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512213907916.png" alt="image-20250512213907916" style="zoom:50%;" />

> You could think of this in a more discrete way: suppose you break up the disk into nn concentric rings, and pick and equal number of points in each ring. The points on the inner rings will be more densely packed than in the outer rings. That's all.

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512214232354.png" alt="image-20250512214232354" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512214621551.png" alt="image-20250512214621551" style="zoom:50%;" />

---

Uniform sampling via rejection sampling

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512215304876.png" alt="image-20250512215304876" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250512215320727.png" alt="image-20250512215320727" style="zoom:50%;" />

---

**Summary**

假设固定总样本数 $N=10^6$：

1. **梯形法（Trapezoid Rule）误差**

   - 在 $k$ 维情况下，如果我们在每个维度上均匀划分网格，使得总的采样点数为 $N=n^k$，那么每维的步长 $h\approx N^{-1/k}$。由于梯形法在一维的误差是 $O(h^2)$，多维时总误差就变为
     $$
     O(h^2) \;=\; O\bigl(N^{-2/d}\bigr).
     $$
     
     
     
   - 随着维度 $k$ 增大，指数 $2/k$ 会变小，因此$ N^{-2/k}$ 上升得越来越慢——换句话说，同样的 $N$，高维下的梯形法误差会 **显著变大**。
   
2. **蒙特卡洛积分（Monte Carlo）误差**

   - 无论是几维，只要你用的是同样 $N$ 个独立随机点，误差始终是
     $$
     O\bigl(N^{-1/2}\bigr).
     $$
     
- 这与维度 **无关**。

------

**结论**

- **低维（如 1D、2D）时**：梯形法（$O(N^{-2/d})$）通常比 Monte Carlo（$O(N^{-1/2})$）收敛得快得多，所以我们更常用它。
- **高维（$k$ 很大）时**：梯形法的收敛率退化严重，而 Monte Carlo 保持 $N^{-1/2}$不变，就变得更高效、更实用。

这样就清楚地说明了：**在相同样本数下，随着维度升高，梯形法的误差会大幅增长，而蒙特卡洛误差则不受维度影响**。

# Lecture 18: Monte Carlo Rendering

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513170855044.png" alt="image-20250513170855044" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513171117338.png" alt="image-20250513171117338" style="zoom:50%;" />

---

**Ray Tracing vs. Rasterization**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513171458566.png" alt="image-20250513171458566" style="zoom:50%;" />

---

**Monte Carlo Integration**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513172112946.png" alt="image-20250513172112946" style="zoom:50%;" />

---

**Law of Large Numbers**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513172947777.png" alt="image-20250513172947777" style="zoom:70%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513173023158.png" alt="image-20250513173023158" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513173032262.png" alt="image-20250513173032262" style="zoom:35%;" />

---

**Biasing**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513173128641.png" alt="image-20250513173128641" style="zoom:50%;" />

---

**Importance sampling**

Where is the best place to take samples?2

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513182621989.png" alt="image-20250513182621989" style="zoom:50%;" />

**Uniform sampling on hemisphere**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513183016531.png" alt="image-20250513183016531" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513183446807.png" alt="image-20250513183446807" style="zoom:50%;" />

---

**Reduce Noise**

Integrate the light source instead of the unit hemisphere at the point $p$

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513184625722.png" alt="image-20250513184625722" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513185143405.png" alt="image-20250513185143405" style="zoom:67%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513185202293.png" alt="image-20250513185202293" style="zoom:67%;" />

---

**Cosine-Weighted Sampling**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513185858168.png" alt="image-20250513185858168" style="zoom:50%;" />

---

**Monte Carlo + Rendering Equation**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513190058654.png" alt="image-20250513190058654" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513190612346.png" alt="image-20250513190612346" style="zoom:50%;" />

---

**Russian roulette**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513190649013.png" alt="image-20250513190649013" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513190659498.png" alt="image-20250513190659498" style="zoom:50%;" />

---

**Summary**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513190902808.png" alt="image-20250513190902808" style="zoom:50%;" />

ChatGPT:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513191213698.png" alt="image-20250513191213698" style="zoom:80%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513191224485.png" alt="image-20250513191224485" style="zoom:80%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250513191233396.png" alt="image-20250513191233396" style="zoom:80%;" />

# Lecture 19: Variance Reduction

> Review:
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529221413672.png" alt="image-20250529221413672" style="zoom:50%;" />

---

**How can we reduce the variance?**

We cannot reduce variance of the *integrand*! （被积函数）

We can only reduce variance of an estimator.

---

**Variance of an Estimator**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529222329233.png" alt="image-20250529222329233" style="zoom:50%;" />

---

**Bias & Consistency**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529222427901.png" alt="image-20250529222427901" style="zoom:50%;" />

Why does it matter?

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529224006787.png" alt="image-20250529224006787" style="zoom:50%;" />

---

**Naive Path Tracing: Which Paths Can We Trace?**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529224110313.png" alt="image-20250529224110313" style="zoom:50%;" />

Naive path tracing misses important phenomena! Formally the result is biased.

> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529224657909.png" alt="image-20250529224657909" style="zoom:50%;" />

We need better strategy!

---

> Review:
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529224741298.png" alt="image-20250529224741298" style="zoom:50%;" />

**Path Space Formulation of Light Transport**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529224921915.png" alt="image-20250529224921915" style="zoom:50%;" />

> 这一张幻灯片的核心，是把渲染问题从“在每个交点沿不同方向积分”（递归渲染方程）提升到“在所有可能的光线路径上做一次全局积分”。下面分几步拆解它的含义：
>
> ------
>
> 1. 什么是“路径空间”？
>
> - **路径（path）**：在光线追踪里，一条路径就是一条从相机（或视点）出发，经由若干次表面交互（散射、反射或折射），最终到达某个光源或无穷远的射线。
> - **路径空间 Ω**：把所有可能的这种“从相机到光源”的路径都视作一个集合，我们称之为路径空间。它是一个无限维的空间，因为不同路径可以有不同的“顶点数量”、不同的折射点位置、不同的方向。
>
> ------
>
> 2. “路径积分”公式
>
> $$
> I \;=\; \int_{\Omega} f(\bar{x}) \,d\mu(\bar{x})
> $$
>
> 
>
> 
>
> - $\bar{x}$ 表示**一条具体的路径**（比如 “相机→点1→点2→光源”），红色箭头指向的文字“one particular path”。
> - $f(\bar{x})$ 表示这条路径**携带的光通量（contribution）**——包括每一次交点的BSDF、几何项和光源辐射强度等的乘积，总的来说就是“这条路到达相机时带来了多少光”。
> - $d\mu(\bar{x})$ 是定义在路径空间上的**测度（measure）**，它告诉我们“在这条路径周围覆盖了多大的一块路径空间”。如果我们把它当成一个概率密度 $p(\bar{x})$来看，那么 $p(\bar{x})\,d\bar{x}$ 就是“随机抽到这条路径”的概率。
>
> 把它想象成把所有可能的路径排成一张“超大表格”，然后对每条路的“权重”$f(\bar{x})$乘以“表格里这条路占多大比例”$d\mu(\bar{x})$，做加和（积分），就得到最终图像中某个像素的光照值。
>
> ------
>
> 3. 和递归渲染方程的对比
>
> - **递归方式**：在每个表面点做一个小积分（对 incoming 方向积分），再递归地“追下一次”。它是把多维积分拆成一维一维的小步算。
> - **路径空间方式**：直接把所有“从头到尾”的路径看成一个整体，在一个公式里一次性定义整个积分。
>
> 二者等价，但后者优势是：
>
> - 我们可以设计**全局重要性采样**，直接在整个路径空间里采样那些“贡献最大的”路径类型（比如优先采样直射、或优先采样关键的光线反弹模式）。
> - 更容易引入混合策略（Multiple Importance Sampling），把不同采样策略的路径分布结合起来、加权。
>
> ------
>
> 4. Monte Carlo Path Tracing
>
> 在实践里，我们不会穷举所有路径，而是用 Monte Carlo：
>
> 1. **抽样**：按某个分布 $p(\bar{x})$（即实现 $\mu$ 的一个归一化版本）随机生成 $N$ 条路径 $\{\bar{x}_i\}$。
>
> 2. **估计**：计算
>
>    $ I = \frac{1}{N}\sum_{i=1}^N \frac{f(\bar{x}_i)}{p(\bar{x}_i)}\,$
>
>    这就是**路径追踪（Path Tracing）**的核心公式——每条路径的贡献除以它被抽到的概率，保证估计无偏。
>
> 随着 NN 增大，这个 Monte Carlo 估计会**一致**地收敛到真正的积分值 II。
>
> ------
>
>  总结
>
> - **路径空间积分**把渲染方程统一看作对“全局路径”做一个高维积分。
> - 这样一来，我们就可以针对整个路径空间做重要性采样、混合采样、甚至 Markov 链蒙特卡洛（如 Metropolis Light Transport）等更智能的采样策略。
> - 虽然公式看上去抽象，但它揭示了“为什么可以设计各种全局光照算法”的数学基础：**一切都是对同一个路径空间积分的不同 Monte Carlo 求解方案**。
>
> 希望这样分层次地拆解，能让你更清楚地看到这页 PPT 想传达的“统一视角”！

Example:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529225421528.png" alt="image-20250529225421528" style="zoom:50%;" />

**Bidirectional Path Tracing**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529225503702.png" alt="image-20250529225503702" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529225543154.png" alt="image-20250529225543154" style="zoom:50%;" />

---

**Metropolis Light transport**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529225715197.png" alt="image-20250529225715197" style="zoom:50%;" />

**Metropolis-Hastings Algorithm (MH)**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529225818871.png" alt="image-20250529225818871" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529230442449.png" alt="image-20250529230442449" style="zoom:50%;" />

---

**Multiple Importance Sampling (MIS)**

![image-20250529230627788](C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529230627788.png)

---

**Sampling Patterns & Variance Reduction**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529230759552.png" alt="image-20250529230759552" style="zoom:50%;" />

**Stratified Sampling**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529230819615.png" alt="image-20250529230819615" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529230850413.png" alt="image-20250529230850413" style="zoom:50%;" />

---

**Low-Discrepancy Sampling**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529230912935.png" alt="image-20250529230912935" style="zoom:50%;" />

We wanna replace random samples with low-discrepancy samples

---

**Hammersley & Halton Points**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529231140672.png" alt="image-20250529231140672" style="zoom:50%;" />

> Wait:
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529231200104.png" alt="image-20250529231200104" style="zoom:50%;" />
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529231243280.png" alt="image-20250529231243280" style="zoom:50%;" />

---

**Blue Noise （蓝纹）**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529231426038.png" alt="image-20250529231426038" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529231439430.png" alt="image-20250529231439430" style="zoom:50%;" />

---

**Poisson Disk Sampling**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529231600330.png" alt="image-20250529231600330" style="zoom:50%;" />

…

---

**Adaptive Blue Noise**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529231959252.png" alt="image-20250529231959252" style="zoom:50%;" />

---

**Sampling from the CDF (cumulation  distribution function)**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529232121642.png" alt="image-20250529232121642" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529232036492.png" alt="image-20250529232036492" style="zoom:50%;" />

---

**Summary**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529232256360.png" alt="image-20250529232256360" style="zoom:50%;" />

> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529232707879.png" alt="image-20250529232707879" style="zoom:67%;" />
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529232723987.png" alt="image-20250529232723987" style="zoom:67%;" />
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529232747983.png" alt="image-20250529232747983" style="zoom:67%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250529232333052.png" alt="image-20250529232333052" style="zoom:50%;" />







