import SwiftUI

class RomManager: ObservableObject {
    func getRoms() -> [String] {
        guard let resourcePath = Bundle.main.resourcePath else { return [] }
        
        do {
            let files = try FileManager.default.contentsOfDirectory(atPath: resourcePath)
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
                        EmulatorView(romFilename: rom)
                    }) {
                        VStack(alignment: .leading) {
                            Text(rom)
                                .font(.headline)
                        }
                    }
                }
            }
            .navigationTitle("Installed ROMs")
        }
    }
}

// Preview provider
struct RomsListView_Previews: PreviewProvider {
    static var previews: some View {
        RomsListView()
    }
} 