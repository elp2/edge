import SwiftUI

class RomManager: ObservableObject {
    func getRoms() -> [String] {
        let romsPath = EmulatorBridge.romsDirectory()
        
        do {
            let files = try FileManager.default.contentsOfDirectory(atPath: romsPath)
            return files.filter { 
                let filename = $0.lowercased()
                return filename.hasSuffix(".gb") && filename != "boot.gb"
            }
        } catch {
            print("Error getting ROMs: \(error)")
            return []
        }
    }
}

struct RomsListView: View {
    @StateObject private var romManager = RomManager()
    
    var body: some View {
        NavigationView {
            List {
                ForEach(romManager.getRoms(), id: \.self) { rom in
                    NavigationLink(destination: {
                        GameScreen(romFilename: rom)
                    }) {
                        VStack(alignment: .leading) {
                            Text(rom)
                                .font(.headline)
                        }
                    }
                }
            }
            .navigationTitle("ROMs")
        }
    }
}

// Preview provider
struct RomsListView_Previews: PreviewProvider {
    static var previews: some View {
        RomsListView()
    }
} 
