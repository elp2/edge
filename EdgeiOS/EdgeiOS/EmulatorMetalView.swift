import SwiftUI
import MetalKit

struct EmulatorMetalView: UIViewRepresentable {
    func makeUIView(context: Context) -> MTKView {
        let mtkView = MTKView()
        mtkView.device = MTLCreateSystemDefaultDevice()
        mtkView.delegate = context.coordinator
        mtkView.clearColor = MTLClearColor(red: 0, green: 0, blue: 0, alpha: 1)
        mtkView.colorPixelFormat = .bgra8Unorm
        mtkView.preferredFramesPerSecond = 60
        context.coordinator.setupMetal(mtkView: mtkView)
        mtkView.preferredFramesPerSecond = 60
        return mtkView
    }

    func updateUIView(_ uiView: MTKView, context: Context) {}

    func makeCoordinator() -> Coordinator {
        Coordinator()
    }

    class Coordinator: NSObject, MTKViewDelegate {
        var device: MTLDevice!
        var commandQueue: MTLCommandQueue!
        var texture: MTLTexture!
        var pipelineState: MTLRenderPipelineState!
        var vertexBuffer: MTLBuffer!
        var frameCount: Int = 0

        func setupMetal(mtkView: MTKView) {
            device = mtkView.device
            commandQueue = device.makeCommandQueue()
            
            // Create texture
            let textureDescriptor = MTLTextureDescriptor.texture2DDescriptor(
                pixelFormat: .bgra8Unorm, width: 160, height: 144, mipmapped: false)
            texture = device.makeTexture(descriptor: textureDescriptor)

            // Create render pipeline
            let library = device.makeDefaultLibrary()!
            let vertexFunction = library.makeFunction(name: "vertexShader")
            let fragmentFunction = library.makeFunction(name: "fragmentShader")

            let pipelineDescriptor = MTLRenderPipelineDescriptor()
            pipelineDescriptor.vertexFunction = vertexFunction
            pipelineDescriptor.fragmentFunction = fragmentFunction
            pipelineDescriptor.colorAttachments[0].pixelFormat = mtkView.colorPixelFormat

            pipelineState = try! device.makeRenderPipelineState(descriptor: pipelineDescriptor)

            // Create vertex buffer for full-screen quad
            let vertices: [Float] = [
                -1,  1, 0, 0,  // position, texcoord
                 1,  1, 1, 0,
                -1, -1, 0, 1,
                 1, -1, 1, 1,
            ]
            vertexBuffer = device.makeBuffer(bytes: vertices, length: vertices.count * MemoryLayout<Float>.size)

            let bridge = EmulatorBridge.sharedInstance()
            bridge.initializeSDL()
            bridge.loadROM("pocket.gb")

        }

        func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {}

        func draw(in view: MTKView) {
            let bridge = EmulatorBridge.sharedInstance()
            let pixels = bridge.pixels()
            
            texture.replace(region: MTLRegionMake2D(0, 0, 160, 144),
                          mipmapLevel: 0, withBytes: pixels, bytesPerRow: 160 * 4)

            guard let commandBuffer = commandQueue.makeCommandBuffer(),
                  let renderPass = view.currentRenderPassDescriptor,
                  let encoder = commandBuffer.makeRenderCommandEncoder(descriptor: renderPass),
                  let drawable = view.currentDrawable else { return }

            encoder.setRenderPipelineState(pipelineState)
            encoder.setVertexBuffer(vertexBuffer, offset: 0, index: 0)
            encoder.setFragmentTexture(texture, index: 0)
            encoder.drawPrimitives(type: .triangleStrip, vertexStart: 0, vertexCount: 4)
            
            encoder.endEncoding()
            commandBuffer.present(drawable)
            commandBuffer.commit()
        }

        deinit {
        }
    }
}

// Preview
struct EmulatorMetalView_Previews: PreviewProvider {
    static var previews: some View {
        EmulatorMetalView()
            .frame(width: 160, height: 144)
    }
}
