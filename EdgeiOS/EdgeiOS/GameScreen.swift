import SwiftUI

struct GameScreen: View {
    let romFilename: String
    @State private var saveStates: [Int] = []
    
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
                
                // Save states list
                if saveStates.isEmpty {
                    VStack {
                        Text("No save states found")
                            .foregroundColor(.secondary)
                            .padding()
                    }
                    .frame(maxWidth: .infinity)
                    .background(Color.gray.opacity(0.1))
                    .cornerRadius(8)
                    .padding(.horizontal)
                } else {
                    ScrollView {
                        LazyVStack(spacing: 8) {
                            ForEach(saveStates, id: \.self) { slot in
                                NavigationLink(destination: EmulatorView(romFilename: romFilename, loadSlot: slot)) {
                                    HStack {
                                        Text("State \(slot)")
                                            .font(.headline)
                                        Spacer()
                                        Image(systemName: "chevron.right")
                                            .foregroundColor(.secondary)
                                    }
                                    .padding()
                                    .background(Color.gray.opacity(0.1))
                                    .cornerRadius(8)
                                }
                                .buttonStyle(PlainButtonStyle())
                            }
                        }
                        .padding(.horizontal)
                    }
                }
            }
            
            Spacer()
        }
        .navigationTitle("")
        .navigationBarTitleDisplayMode(.inline)
        .onAppear {
            let bridge = EmulatorBridge.sharedInstance()
            bridge.loadROM(romFilename)
            loadSaveStates()
        }
    }
    
    private func loadSaveStates() {
        let bridge = EmulatorBridge.sharedInstance()
        let states = bridge.getSaveStates()
        print("GameScreen: Found \(states.count) save states for \(romFilename)")
        for state in states {
            print("GameScreen: Save state slot \(state.intValue)")
        }
        saveStates = states.map { $0.intValue }
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
