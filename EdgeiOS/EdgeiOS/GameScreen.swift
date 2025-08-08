import SwiftUI

struct GameScreen: View {
    let romFilename: String
    @State private var saveStates: [SaveStateWrapper] = []
    
    var body: some View {
        VStack(alignment: .leading, spacing: 20) {
            titleView
            mainActionButton
            saveStatesSection
            
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
    
    private var titleView: some View {
        Text(romFilename)
            .font(.largeTitle)
            .fontWeight(.bold)
            .padding(.horizontal)
            .padding(.top)
    }
    
    private var hasMainState: Bool {
        return saveStates.contains { $0.getSlot() == 0 }
    }
    
    private var mainActionButton: some View {
        Group {
            if hasMainState {
                NavigationLink(destination: EmulatorView(romFilename: romFilename, loadSlot: 0)) {
                    HStack {
                        Image(systemName: "arrow.clockwise")
                        Text("Resume Game")
                    }
                    .font(.headline)
                    .foregroundColor(.white)
                    .frame(maxWidth: .infinity)
                    .padding()
                    .background(Color.green)
                    .cornerRadius(10)
                }
                .padding(.horizontal)
                .buttonStyle(PlainButtonStyle())
            } else {
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
            }
        }
    }
    
    private var saveStatesSection: some View {
        VStack(alignment: .leading, spacing: 10) {
            Text("Manual Save States")
                .font(.title2)
                .fontWeight(.semibold)
                .padding(.horizontal)
            
            saveStatesList
        }
    }
    
    private var saveStatesList: some View {
        Group {
            if saveStates.isEmpty {
                emptyStateView
            } else {
                saveStatesListView
            }
        }
    }
    
    private var emptyStateView: some View {
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
    
    private var saveStatesListView: some View {
        ScrollView {
            LazyVStack(spacing: 8) {
                ForEach(saveStates, id: \.slot) { state in
                    saveStateRow(state)
                }
            }
            .padding(.horizontal)
        }
    }
    
    private func saveStateRow(_ state: SaveStateWrapper) -> some View {
        NavigationLink(destination: EmulatorView(romFilename: romFilename, loadSlot: Int(state.getSlot()))) {
            HStack(spacing: 12) {
                // Screenshot
                if let screenshot = state.getScreenshotImage() {
                    Image(uiImage: screenshot)
                        .resizable()
                        .aspectRatio(contentMode: .fit)
                        .frame(width: 160, height: 144)
                        .cornerRadius(6)
                } else {
                    Rectangle()
                        .fill(Color.pink.opacity(0.3))
                        .frame(width: 160, height: 144)
                        .cornerRadius(6)
                        .overlay(
                            Image(systemName: "photo")
                                .foregroundColor(.gray)
                        )
                }
                
                // State info
                VStack(alignment: .leading, spacing: 4) {
                    if let saveDate = state.getSaveDate() {
                        Text(formatRelativeTime(saveDate))
                            .font(.headline)
                        Text(formatDate(saveDate))
                            .font(.caption2)
                            .foregroundColor(.secondary)
                    } else {
                        Text(state.getSlot() == 0 ? "Main Save" : "State \(state.getSlot())")
                            .font(.headline)
                        Text("Unknown date")
                            .font(.caption2)
                            .foregroundColor(.secondary)
                    }
                }
                
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
    
    private func loadSaveStates() {
        let bridge = EmulatorBridge.sharedInstance()
        let states = bridge.getSaveStates()
        saveStates = states
    }
    
    private func formatDate(_ date: Date) -> String {
        let formatter = DateFormatter()
        formatter.dateFormat = "MM/dd/yy HH:mm:ss"
        return formatter.string(from: date)
    }
    
    private func formatRelativeTime(_ date: Date) -> String {
        let now = Date()
        let timeInterval = now.timeIntervalSince(date)
        
        if timeInterval < 60 {
            return "Just now"
        } else if timeInterval < 3600 { // Less than 1 hour
            let minutes = Int(timeInterval / 60)
            return "\(minutes) min\(minutes == 1 ? "" : "s") ago"
        } else if timeInterval < 86400 { // Less than 1 day
            let hours = Int(timeInterval / 3600)
            return "\(hours) hour\(hours == 1 ? "" : "s") ago"
        } else if timeInterval < 2592000 { // Less than 30 days
            let days = Int(timeInterval / 86400)
            return "\(days) day\(days == 1 ? "" : "s") ago"
        } else {
            let months = Int(timeInterval / 2592000)
            return "\(months) month\(months == 1 ? "" : "s") ago"
        }
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
