import SwiftUI

struct EmulatorView: View {
    let romFilename: String
    private let bridge: EmulatorBridge
    @State private var loadError: Error?
    private let abButtonSize: CGFloat = 75
    
    init(romFilename: String) {
        self.romFilename = romFilename
        self.bridge = EmulatorBridge.sharedInstance()
    }
    
    var body: some View {
        VStack(spacing: 0) {
            GeometryReader { geometry in
                EmulatorMetalView()
                    .frame(width: geometry.size.width)
                    .frame(height: geometry.size.width * (144.0/160.0))
            }
            
            HStack(spacing: 50) {
                DirectionalPadView()
                
                HStack(spacing: 30) {                    
                    Button("B") { }
                        .frame(width: abButtonSize, height: abButtonSize)
                        .background(Color.red)
                        .foregroundColor(.white)
                        .clipShape(Circle())
                        .simultaneousGesture(
                            DragGesture(minimumDistance: 0)
                                .onChanged { _ in
                                    bridge.buttonB = true;
                                }
                                .onEnded { _ in
                                    bridge.buttonB = false;
                                }
                        )
                    Button("A") { }
                        .frame(width: abButtonSize, height: abButtonSize)
                        .background(Color.red)
                        .foregroundColor(.white)
                        .clipShape(Circle())
                        .offset(y: -30)
                        .simultaneousGesture(
                            DragGesture(minimumDistance: 0)
                                .onChanged { _ in
                                    bridge.buttonA = true;
                                }
                                .onEnded { _ in
                                    bridge.buttonA = false;
                                }
                        )
                }
            }
            .padding(.horizontal, 10)
            .padding(.vertical, 50)
            
            HStack(spacing: 30) {
                Button("Select") { }
                    .frame(width: 60, height: 30)
                    .background(Color.gray)
                    .foregroundColor(.white)
                    .cornerRadius(15)
                    .simultaneousGesture(
                        DragGesture(minimumDistance: 0)
                            .onChanged { _ in
                                bridge.buttonSelect = true;
                            }
                            .onEnded { _ in
                                bridge.buttonSelect = false;
                            }
                    )
                
                Button("Start") { }
                    .frame(width: 60, height: 30)
                    .background(Color.gray)
                    .foregroundColor(.white)
                    .cornerRadius(15)
                    .simultaneousGesture(
                        DragGesture(minimumDistance: 0)
                            .onChanged { _ in
                                bridge.buttonStart = true;
                            }
                            .onEnded { _ in
                                bridge.buttonStart = false;
                            }
                    )
            }
            .padding(.bottom, 30)
        }
        .onAppear {
            bridge.loadROM(romFilename)
        }
        .onDisappear {
            bridge.endEmulator()
        }
        .alert("Failed to load ROM", isPresented: .constant(loadError != nil)) {
            Button("OK", role: .cancel) { loadError = nil }
        } message: {
            if let error = loadError {
                Text(error.localizedDescription)
            }
        }
    }
}

// Preview provider
struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        EmulatorView(romFilename: "pocket.gb")
    }
}
