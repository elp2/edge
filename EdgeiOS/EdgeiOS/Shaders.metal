#include <metal_stdlib>
using namespace metal;

struct VertexOut {
    float4 position [[position]];
    float2 texCoord;
};

vertex VertexOut vertexShader(uint vertexID [[vertex_id]],
                             const device float4* vertices [[buffer(0)]]) {
    VertexOut out;
    float4 position = float4(vertices[vertexID].xy, 0, 1);
    out.position = position;
    out.texCoord = vertices[vertexID].zw;
    return out;
}

fragment float4 fragmentShader(VertexOut in [[stage_in]],
                             texture2d<float> tex [[texture(0)]]) {
    constexpr sampler s(address::clamp_to_edge, filter::nearest);
    return tex.sample(s, in.texCoord);
} 
