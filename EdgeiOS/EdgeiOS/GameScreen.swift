import SwiftUI

struct GameScreen: View {
    let romFilename: String
    
    var body: some View {
        VStack(alignment: .leading, spacing: 20) {
            // Title
            Text(romFilename)
                .font(.largeTitle)
                .fontWeight(.bold)
                .padding(.horizontal)
                .padding(.top)
            
            // Start Game Button
            NavigationLink(destination: EmulatorView(romFilename: romFilename)) {
                HStack {
                    Image(systemName: "play.fill")
                    Text("Start Game")
                }
                .font(.headline)
                .foregroundColor(.white)
                .frame(maxWidth: .infinity)
                .padding()
                .background(Color.blue)
                .cornerRadius(10)
            }
            .padding(.horizontal)
            .buttonStyle(PlainButtonStyle())
            
            // Save States Section
            VStack(alignment: .leading, spacing: 10) {
                Text("Save States")
                    .font(.title2)
                    .fontWeight(.semibold)
                    .padding(.horizontal)
                
                // Placeholder for save states list
                VStack {
                    Text("No save states found")
                        .foregroundColor(.secondary)
                        .padding()
                }
                .frame(maxWidth: .infinity)
                .background(Color.gray.opacity(0.1))
                .cornerRadius(8)
                .padding(.horizontal)
            }
            
            Spacer()
        }
        .navigationTitle("")
        .navigationBarTitleDisplayMode(.inline)
    }
}

// Preview provider
struct GameScreen_Previews: PreviewProvider {
    static var previews: some View {
        NavigationView {
            GameScreen(romFilename: "example.gb")
        }
    }
} 