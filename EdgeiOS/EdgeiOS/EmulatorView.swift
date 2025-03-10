import SwiftUI

struct EmulatorView: View {
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
