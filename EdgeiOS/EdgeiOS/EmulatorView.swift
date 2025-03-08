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
                    HStack(spacing: 30) {
                        Button("←") { }
                        Button("→") { }
                    }
                    Button("↓") { }
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
                            Button("B") { }
                                .frame(width: 50, height: 50)
                                .background(Color.red)
                                .foregroundColor(.white)
                                .clipShape(Circle())
                        }
                    }
                    
                    // Start and Select buttons
                    HStack(spacing: 30) {
                        Button("Select") { }
                            .frame(width: 60, height: 30)
                            .background(Color.gray)
                            .foregroundColor(.white)
                            .cornerRadius(15)
                        
                        Button("Start") { }
                            .frame(width: 60, height: 30)
                            .background(Color.gray)
                            .foregroundColor(.white)
                            .cornerRadius(15)
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
