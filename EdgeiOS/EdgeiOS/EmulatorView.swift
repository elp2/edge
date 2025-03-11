import SwiftUI

struct EmulatorView: View {
    let romFilename: String
    private let bridge: EmulatorBridge
    @State private var loadError: Error?
    
    init(romFilename: String) {
        self.romFilename = romFilename
        self.bridge = EmulatorBridge.sharedInstance()
    }
    
    var body: some View {
        VStack(spacing: 20) {
            GeometryReader { geometry in
                EmulatorMetalView()
                    .frame(width: geometry.size.width)
                    .frame(height: geometry.size.width * (144.0 / 160.0))
            }
            HStack(spacing: 40) {
                DirectionalPadView()
                VStack(spacing: 30) {
                    // Action buttons
                    HStack(spacing: 20) {
                        // B and A buttons
                        HStack(spacing: 30) {
                            Button("A") { }
                                .frame(width: 50, height: 50)
                                .background(Color.red)
                                .foregroundColor(.white)
                                .clipShape(Circle())
                                .simultaneousGesture(
                                    DragGesture(minimumDistance: 0)
                                        .onChanged { _ in
                                            bridge.buttonA = true;
                                        }
                                        .onEnded { _ in
                                            bridge.buttonA = false;
                                        }
                                )
                            Button("B") { }
                                .frame(width: 50, height: 50)
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
                        }
                    }
                    
                    // Start and Select buttons
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

                        Button(action: {}) {
                            Text("Start")
                                .frame(width: 60, height: 30)
                                .background(Color.gray)
                                .foregroundColor(.white)
                                .cornerRadius(15)
                        }
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
                }
                .padding()
            }
        }
        .onAppear {
            do {
                try bridge.loadROM(romFilename)
            } catch {
                print("Failed to start emulator: \(error)")
                loadError = error
            }
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
