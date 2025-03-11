import SwiftUI

@main
struct EdgeiOSApp: App {
    init() {
        EmulatorBridge.sharedInstance().initializeSDL()
    }

    var body: some Scene {
        WindowGroup {
            RomsListView()
        }
    }
}
