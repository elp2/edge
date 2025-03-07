import SwiftUI

struct ContentView: View {
    var body: some View {
        VStack(spacing: 20) {
            // Emulator display
            EmulatorMetalView()
                .frame(width: 160, height: 144)
                .border(Color.gray)
            
            // D-Pad (placeholder for now)
            VStack(spacing: 10) {
                Button("↑") { }
                HStack(spacing: 30) {
                    Button("←") { }
                    Button("→") { }
                }
                Button("↓") { }
            }
            
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

// Preview provider
struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
