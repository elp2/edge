import SwiftUI

@main
struct EdgeiOSApp: App {
    init() {
        EmulatorBridge.sharedInstance().initializeSDL()
        EmulatorBridge.createDirectories()
        EmulatorBridge.copyBundledROMs()
    }

    var body: some Scene {
        WindowGroup {
            RomsListView()
        }
    }
}
