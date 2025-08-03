import SwiftUI
import MetalKit
import UIKit

protocol EmulatorMetalViewDelegate {
    func didSaveState()
    func didLoadState()
    func didGoBackInMemory()
    func didRequestLoadStateScreen()
}

struct EmulatorMetalView: UIViewRepresentable {
    var delegate: EmulatorMetalViewDelegate?
    
    func makeUIView(context: Context) -> MTKView {
        let mtkView = MTKView()
        mtkView.device = MTLCreateSystemDefaultDevice()
        mtkView.delegate = context.coordinator
        mtkView.clearColor = MTLClearColor(red: 0, green: 0, blue: 0, alpha: 1)
        mtkView.colorPixelFormat = .bgra8Unorm
        mtkView.preferredFramesPerSecond = 60
        context.coordinator.setupMetal(mtkView: mtkView)
        mtkView.preferredFramesPerSecond = 60

        // Add a UIKit double tap recognizer so we can get coordinates of the tap.
        let doubleTapRecognizer = UITapGestureRecognizer(
            target: context.coordinator,
            action: #selector(Coordinator.handleDoubleTap(_:))
        )
        doubleTapRecognizer.numberOfTapsRequired = 2
        mtkView.addGestureRecognizer(doubleTapRecognizer)
        
        // Add pan gesture recognizer for custom swipe detection with distance threshold
        let panRecognizer = UIPanGestureRecognizer(
            target: context.coordinator,
            action: #selector(Coordinator.handlePan(_:))
        )
        mtkView.addGestureRecognizer(panRecognizer)
        
        return mtkView
    }

    func updateUIView(_ uiView: MTKView, context: Context) {
        context.coordinator.delegate = delegate
    }

    func makeCoordinator() -> Coordinator {
        Coordinator(delegate: delegate)
    }

    class Coordinator: NSObject, MTKViewDelegate {
        var device: MTLDevice!
        var commandQueue: MTLCommandQueue!
        var texture: MTLTexture!
        var pipelineState: MTLRenderPipelineState!
        var vertexBuffer: MTLBuffer!
        var frameCount: Int = 0
        var delegate: EmulatorMetalViewDelegate?
        
        init(delegate: EmulatorMetalViewDelegate?) {
            self.delegate = delegate
            super.init()
        }

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
        }

        func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {}

        func draw(in view: MTKView) {
            let bridge = EmulatorBridge.sharedInstance()
            bridge.advanceOneFrame()
            let pixels = bridge.pixels()
            let pixelsCopy = UnsafeMutablePointer<UInt32>.allocate(capacity: 160 * 144)
            memcpy(pixelsCopy, pixels, 160 * 144 * 4)

            texture.replace(region: MTLRegionMake2D(0, 0, 160, 144),
                          mipmapLevel: 0, withBytes: pixelsCopy, bytesPerRow: 160 * 4)

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

        @objc func handleDoubleTap(_ recognizer: UITapGestureRecognizer) {
            guard recognizer.state == .ended else { return }
            guard let view = recognizer.view else { return }
            // TODO: Implement double tap (options).
        }

        @objc func handlePan(_ gesture: UIPanGestureRecognizer) {
            guard gesture.state == .ended else { return }
            guard let view = gesture.view else { return }
            
            let translation = gesture.translation(in: view)
            let velocity = gesture.velocity(in: view)
            
            let minDistance = min(view.bounds.width, view.bounds.height) / 3.0
            
            let distance = sqrt(translation.x * translation.x + translation.y * translation.y)
            guard distance >= minDistance else { return }
            
            let absX = abs(translation.x)
            let absY = abs(translation.y)
            
            if absY > absX {
                // Vertical swipe
                if translation.y > 0 {
                    // Down swipe
                    delegate?.didSaveState()
                } else {
                    // Up swipe
                    delegate?.didLoadState()
                }
            } else {
                // Horizontal swipe
                if translation.x > 0 {
                    // Right swipe
                    delegate?.didRequestLoadStateScreen()
                } else {
                    // Left swipe
                    delegate?.didGoBackInMemory()
                }
            }
        }

        deinit {
        }
    }
}

struct EmulatorMetalView_Previews: PreviewProvider {
    static var previews: some View {
        EmulatorMetalView()
            .frame(width: 160, height: 144)
    }
}
