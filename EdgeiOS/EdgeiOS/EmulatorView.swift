import SwiftUI

struct EmulatorView: View {
    var body: some View {
        VStack(spacing: 20) {
            GeometryReader { geometry in
                EmulatorMetalView()
                    .frame(maxWidth: .infinity)
                    .frame(height: geometry.size.width)
            }
            HStack(spacing: 40) {
                // D-Pad (placeholder for now)
                VStack(spacing: 10) {
                    Button("↑") { }
                        .simultaneousGesture(
                            DragGesture(minimumDistance: 0)
                                .onChanged { _ in
                                    EmulatorBridge.sharedInstance().dpadUp = true;
                                }
                                .onEnded { _ in
                                    EmulatorBridge.sharedInstance().dpadUp = false;
                                }
                        )
                    HStack(spacing: 30) {
                        Button("←") { }
                            .simultaneousGesture(
                                DragGesture(minimumDistance: 0)
                                    .onChanged { _ in
                                        EmulatorBridge.sharedInstance().dpadLeft = true;
                                    }
                                    .onEnded { _ in
                                        EmulatorBridge.sharedInstance().dpadLeft = false;
                                    }
                            )
                        Button("→") { }
                            .simultaneousGesture(
                                DragGesture(minimumDistance: 0)
                                    .onChanged { _ in
                                        EmulatorBridge.sharedInstance().dpadRight = true;
                                    }
                                    .onEnded { _ in
                                        EmulatorBridge.sharedInstance().dpadRight = false;
                                    }
                            )
                    }
                    Button("↓") { }
                        .simultaneousGesture(
                            DragGesture(minimumDistance: 0)
                                .onChanged { _ in
                                    EmulatorBridge.sharedInstance().dpadDown = true;
                                }
                                .onEnded { _ in
                                    EmulatorBridge.sharedInstance().dpadDown = false;
                                }
                        )
                }
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
                                            EmulatorBridge.sharedInstance().buttonA = true;
                                        }
                                        .onEnded { _ in
                                            EmulatorBridge.sharedInstance().buttonA = false;
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
                                            EmulatorBridge.sharedInstance().buttonB = true;
                                        }
                                        .onEnded { _ in
                                            EmulatorBridge.sharedInstance().buttonB = false;
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
                                        EmulatorBridge.sharedInstance().buttonSelect = true;
                                    }
                                    .onEnded { _ in
                                        EmulatorBridge.sharedInstance().buttonSelect = false;
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
                                    EmulatorBridge.sharedInstance().buttonStart = true;
                                }
                                .onEnded { _ in
                                    EmulatorBridge.sharedInstance().buttonStart = false;
                                }
                        )
                    }
                }
                .padding()
            }
        }
    }
}

// Preview provider
struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        EmulatorView()
    }
}
