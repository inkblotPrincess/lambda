/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 */

namespace lambda::math
{
    template<class value_type>
    struct vec2
    {
        static_assert(std::is_arithmetic_v<value_type>, "expected arithmetic value type for vec2");

        value_type X;
        value_type Y;

        constexpr explicit vec2() : vec2(value_type{}, value_type{}) { }
        constexpr explicit vec2(value_type XVal, value_type YVal) : X{XVal}, Y{YVal} { }
        constexpr explicit vec2(value_type XY) : vec2(XY, XY) { }

        [[nodiscard]] static constexpr auto dot(vec2 const& Lhs, vec2 const& Rhs) noexcept -> value_type 
        { 
            return (Lhs.X * Rhs.X) + (Lhs.Y * Rhs.Y); 
        }

        [[nodiscard]] auto length() noexcept -> value_type
        requires std::floating_point<value_type>
        {
            auto const LengthSq = dot(*this, *this);
            expect(LengthSq != value_type{}, "zero length^2 in vec2::length");

            return std::sqrt(LengthSq);
        }

        constexpr auto operator+=(vec2 const& Rhs) noexcept -> vec2& { X += Rhs.X; Y += Rhs.Y; return *this; }
        constexpr auto operator-=(vec2 const& Rhs) noexcept -> vec2& { X -= Rhs.X; Y -= Rhs.Y; return *this; }
        constexpr auto operator*=(vec2 const& Rhs) noexcept -> vec2& { X *= Rhs.X; Y *= Rhs.Y; return *this; }
        constexpr auto operator/=(vec2 const& Rhs) noexcept -> vec2& { X /= Rhs.X; Y /= Rhs.Y; return *this; }
        
        constexpr auto operator*=(value_type Scalar) noexcept -> vec2& { X *= Scalar; Y *= Scalar; return *this; }
        constexpr auto operator/=(value_type Scalar) noexcept -> vec2& { X /= Scalar; Y /= Scalar; return *this; }

        [[nodiscard]] constexpr auto operator+() const noexcept -> vec2 { return *this; }
        [[nodiscard]] constexpr auto operator-() const noexcept -> vec2 { return vec2{-X, -Y}; }

        [[nodiscard]] friend constexpr auto operator+(vec2 Lhs, vec2 const& Rhs) noexcept -> vec2 { return Lhs += Rhs; }
        [[nodiscard]] friend constexpr auto operator-(vec2 Lhs, vec2 const& Rhs) noexcept -> vec2 { return Lhs -= Rhs; }
        [[nodiscard]] friend constexpr auto operator*(vec2 Lhs, vec2 const& Rhs) noexcept -> vec2 { return Lhs *= Rhs; }
        [[nodiscard]] friend constexpr auto operator/(vec2 Lhs, vec2 const& Rhs) noexcept -> vec2 { return Lhs /= Rhs; }

        [[nodiscard]] friend constexpr auto operator*(vec2 Lhs, value_type Scalar) noexcept -> vec2 { return Lhs *= Scalar; }
        [[nodiscard]] friend constexpr auto operator*(value_type Scalar, vec2 Rhs) noexcept -> vec2 { return Rhs *= Scalar; }
        [[nodiscard]] friend constexpr auto operator/(vec2 Lhs, value_type Scalar) noexcept -> vec2 { return Lhs /= Scalar; }

        [[nodiscard]] friend constexpr auto operator==(vec2 const&, vec2 const&) noexcept -> bool = default;
    };

    template<class value_type>
    struct vec3
    {
        static_assert(std::is_arithmetic_v<value_type>, "expected arithmetic value type for vec3");

        value_type X;
        value_type Y;
        value_type Z;

        constexpr explicit vec3() : vec3(value_type{}, value_type{}, value_type{}) { }
        constexpr explicit vec3(value_type XVal, value_type YVal, value_type ZVal) : X{XVal}, Y{YVal}, Z{ZVal} { }
        constexpr explicit vec3(value_type XVal, vec2<value_type> YZ) : vec3(XVal, YZ.X, YZ.Y) { }
        constexpr explicit vec3(vec2<value_type> XY, value_type ZVal) : vec3(XY.X, XY.Y, ZVal) { }
        constexpr explicit vec3(value_type XYZ) : vec3(XYZ, XYZ, XYZ) { }

        [[nodiscard]] static constexpr auto dot(vec3 const& Lhs, vec3 const& Rhs) noexcept -> value_type 
        { 
            return (Lhs.X * Rhs.X) + (Lhs.Y * Rhs.Y) + (Lhs.Z * Rhs.Z); 
        }

        [[nodiscard]] static constexpr auto cross(vec3 const& Lhs, vec3 const& Rhs) noexcept -> vec3
        {
            return vec3{
                Lhs.Y * Rhs.Z - Lhs.Z * Rhs.Y,
                Lhs.Z * Rhs.X - Lhs.X * Rhs.Z,
                Lhs.X * Rhs.Y - Lhs.Y * Rhs.X
            };
        }

        [[nodiscard]] auto length() noexcept -> value_type
        requires std::floating_point<value_type>
        {
            auto const LengthSq = dot(*this, *this);
            expect(LengthSq != value_type{}, "zero length^2 in vec3::length");

            return std::sqrt(LengthSq);
        }

        [[nodiscard]] constexpr auto xy() const noexcept -> vec2<value_type> { return vec2<value_type>{X, Y}; }
        [[nodiscard]] constexpr auto yz() const noexcept -> vec2<value_type> { return vec2<value_type>{Y, Z}; }

        constexpr auto operator+=(vec3 const& Rhs) noexcept -> vec3& { X += Rhs.X; Y += Rhs.Y; Z += Rhs.Z; return *this; }
        constexpr auto operator-=(vec3 const& Rhs) noexcept -> vec3& { X -= Rhs.X; Y -= Rhs.Y; Z -= Rhs.Z; return *this; }
        constexpr auto operator*=(vec3 const& Rhs) noexcept -> vec3& { X *= Rhs.X; Y *= Rhs.Y; Z *= Rhs.Z; return *this; }
        constexpr auto operator/=(vec3 const& Rhs) noexcept -> vec3& { X /= Rhs.X; Y /= Rhs.Y; Z /= Rhs.Z; return *this; }
        
        constexpr auto operator*=(value_type Scalar) noexcept -> vec3& { X *= Scalar; Y *= Scalar; Z *= Scalar; return *this; }
        constexpr auto operator/=(value_type Scalar) noexcept -> vec3& { X /= Scalar; Y /= Scalar; Z /= Scalar; return *this; }

        [[nodiscard]] constexpr auto operator+() const noexcept -> vec3 { return *this; }
        [[nodiscard]] constexpr auto operator-() const noexcept -> vec3 { return vec3{-X, -Y, -Z}; }

        [[nodiscard]] friend constexpr auto operator+(vec3 Lhs, vec3 const& Rhs) noexcept -> vec3 { return Lhs += Rhs; }
        [[nodiscard]] friend constexpr auto operator-(vec3 Lhs, vec3 const& Rhs) noexcept -> vec3 { return Lhs -= Rhs; }
        [[nodiscard]] friend constexpr auto operator*(vec3 Lhs, vec3 const& Rhs) noexcept -> vec3 { return Lhs *= Rhs; }
        [[nodiscard]] friend constexpr auto operator/(vec3 Lhs, vec3 const& Rhs) noexcept -> vec3 { return Lhs /= Rhs; }

        [[nodiscard]] friend constexpr auto operator*(vec3 Lhs, value_type Scalar) noexcept -> vec3 { return Lhs *= Scalar; }
        [[nodiscard]] friend constexpr auto operator*(value_type Scalar, vec3 Rhs) noexcept -> vec3 { return Rhs *= Scalar; }
        [[nodiscard]] friend constexpr auto operator/(vec3 Lhs, value_type Scalar) noexcept -> vec3 { return Lhs /= Scalar; }

        [[nodiscard]] friend constexpr auto operator==(vec3 const&, vec3 const&) noexcept -> bool = default;
    };

    template<class value_type>
    struct vec4
    {
        static_assert(std::is_arithmetic_v<value_type>, "expected arithmetic value type for vec4");

        value_type X;
        value_type Y;
        value_type Z;
        value_type W;

        constexpr explicit vec4() : vec4(value_type{}, value_type{}, value_type{}, value_type{}) { }
        constexpr explicit vec4(value_type XVal, value_type YVal, value_type ZVal, value_type WVal) : X{XVal}, Y{YVal}, Z{ZVal}, W{WVal} { }
        constexpr explicit vec4(value_type XVal, value_type YVal, vec2<value_type> ZW) : vec4(XVal, YVal, ZW.X, ZW.Y) { }
        constexpr explicit vec4(value_type XVal, vec2<value_type> YZ, value_type WVal) : vec4(XVal, YZ.X, YZ.Y, WVal) { }
        constexpr explicit vec4(vec2<value_type> XY, value_type ZVal, value_type WVal) : vec4(XY.X, XY.Y, ZVal, WVal) { }
        constexpr explicit vec4(vec2<value_type> XY, vec2<value_type> ZW) : vec4(XY.X, XY.Y, ZW.X, ZW.Y) { }
        constexpr explicit vec4(value_type XVal, vec3<value_type> YZW) : vec4(XVal, YZW.X, YZW.Y, YZW.Z) { }
        constexpr explicit vec4(vec3<value_type> XYZ, value_type WVal) : vec4(XYZ.X, XYZ.Y, XYZ.Z, WVal) { }
        constexpr explicit vec4(value_type XYZW) : vec4(XYZW, XYZW, XYZW, XYZW) { }

        [[nodiscard]] static constexpr auto dot(vec4 const& Lhs, vec4 const& Rhs) noexcept -> value_type 
        { 
            return (Lhs.X * Rhs.X) + (Lhs.Y * Rhs.Y) + (Lhs.Z * Rhs.Z) + (Lhs.W * Rhs.W); 
        }

        [[nodiscard]] auto length() noexcept -> value_type
        requires std::floating_point<value_type>
        {
            auto const LengthSq = dot(*this, *this);
            expect(LengthSq != value_type{}, "zero length^2 in vec4::length");

            return std::sqrt(LengthSq);
        }

        [[nodiscard]] constexpr auto xy() const noexcept -> vec2<value_type> { return vec2<value_type>{X, Y}; }
        [[nodiscard]] constexpr auto yz() const noexcept -> vec2<value_type> { return vec2<value_type>{Y, Z}; }
        [[nodiscard]] constexpr auto zw() const noexcept -> vec2<value_type> { return vec2<value_type>{Z, W}; }

        [[nodiscard]] constexpr auto xyz() const noexcept -> vec3<value_type> { return vec3<value_type>{X, Y, Z}; }
        [[nodiscard]] constexpr auto yzw() const noexcept -> vec3<value_type> { return vec3<value_type>{Y, Z, W}; }

        constexpr auto operator+=(vec4 const& Rhs) noexcept -> vec4& { X += Rhs.X; Y += Rhs.Y; Z += Rhs.Z; W += Rhs.W; return *this; }
        constexpr auto operator-=(vec4 const& Rhs) noexcept -> vec4& { X -= Rhs.X; Y -= Rhs.Y; Z -= Rhs.Z; W -= Rhs.W; return *this; }
        constexpr auto operator*=(vec4 const& Rhs) noexcept -> vec4& { X *= Rhs.X; Y *= Rhs.Y; Z *= Rhs.Z; W *= Rhs.W; return *this; }
        constexpr auto operator/=(vec4 const& Rhs) noexcept -> vec4& { X /= Rhs.X; Y /= Rhs.Y; Z /= Rhs.Z; W /= Rhs.W; return *this; }
        
        constexpr auto operator*=(value_type Scalar) noexcept -> vec4& { X *= Scalar; Y *= Scalar; Z *= Scalar; W *= Scalar; return *this; }
        constexpr auto operator/=(value_type Scalar) noexcept -> vec4& { X /= Scalar; Y /= Scalar; Z /= Scalar; W /= Scalar; return *this; }

        [[nodiscard]] constexpr auto operator+() const noexcept -> vec4 { return *this; }
        [[nodiscard]] constexpr auto operator-() const noexcept -> vec4 { return vec4{-X, -Y, -Z, -W}; }

        [[nodiscard]] friend constexpr auto operator+(vec4 Lhs, vec4 const& Rhs) noexcept -> vec4 { return Lhs += Rhs; }
        [[nodiscard]] friend constexpr auto operator-(vec4 Lhs, vec4 const& Rhs) noexcept -> vec4 { return Lhs -= Rhs; }
        [[nodiscard]] friend constexpr auto operator*(vec4 Lhs, vec4 const& Rhs) noexcept -> vec4 { return Lhs *= Rhs; }
        [[nodiscard]] friend constexpr auto operator/(vec4 Lhs, vec4 const& Rhs) noexcept -> vec4 { return Lhs /= Rhs; }

        [[nodiscard]] friend constexpr auto operator*(vec4 Lhs, value_type Scalar) noexcept -> vec4 { return Lhs *= Scalar; }
        [[nodiscard]] friend constexpr auto operator*(value_type Scalar, vec4 Rhs) noexcept -> vec4 { return Rhs *= Scalar; }
        [[nodiscard]] friend constexpr auto operator/(vec4 Lhs, value_type Scalar) noexcept -> vec4 { return Lhs /= Scalar; }

        [[nodiscard]] friend constexpr auto operator==(vec4 const&, vec4 const&) noexcept -> bool = default;
    };

    using vec2i = vec2<std::int32_t>;
    using vec2u = vec2<std::uint32_t>;
    using vec2f = vec2<float>;
    using vec2d = vec2<double>;

    using vec3i = vec3<std::int32_t>;
    using vec3u = vec3<std::uint32_t>;
    using vec3f = vec3<float>;
    using vec3d = vec3<double>;

    using vec4i = vec4<std::int32_t>;
    using vec4u = vec4<std::uint32_t>;
    using vec4f = vec4<float>;
    using vec4d = vec4<double>;
} // namespace lambda::math