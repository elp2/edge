import SwiftUI

struct EmulatorView: View {
    let romFilename: String
    let loadSlot: Int?
    private let bridge: EmulatorBridge
    @State private var loadError: Error?
    @State private var showToast = false
    @State private var toastMessage = ""
    private let abButtonSize: CGFloat = 75
    private let impactGenerator = UIImpactFeedbackGenerator(style: .light)
    @Environment(\.verticalSizeClass) var verticalSizeClass
    
    init(romFilename: String, loadSlot: Int? = nil) {
        self.romFilename = romFilename
        self.loadSlot = loadSlot
        self.bridge = EmulatorBridge.sharedInstance()
    }
    
    var body: some View {
        Group {
            if verticalSizeClass == .compact {
                            landscapeView
                        } else {
                            portraitView
                        }
        }
            .onAppear {
                if let slot = loadSlot {
                    bridge.loadState(Int32(slot))
                }
                bridge.startEmulator()
            }
            .onDisappear {
                bridge.endEmulator()
            }
            .alert("Failed to load ROM", isPresented: .constant(loadError != nil)) {
                    Button("OK", role: .cancel) { loadError = nil }
                } message: {
                    if let error = loadError {
                        Text(error.localizedDescription)
                    }
                }
            .navigationBarHidden(verticalSizeClass == .compact)
            .navigationBarTitleDisplayMode(.inline)
            .statusBar(hidden: verticalSizeClass == .compact)
    }

    @ViewBuilder
    private var landscapeView: some View {
        HStack {
            DirectionalPadView()
            GeometryReader { geometry in
                EmulatorMetalView(delegate: self)
                    .frame(width: geometry.size.height * (160.0/144.0))
                    .frame(height: geometry.size.height)
            }
            VStack {
                abButtons
                    .padding(.top, 50)
                    .padding(.vertical, 50)
                selectStartButtons
            }
            .overlay(
                // Toast positioned absolutely above the A/B buttons
                VStack {
                    Spacer()
                    HStack {
                        Spacer()
                        Text(toastMessage)
                            .foregroundColor(.white)
                            .padding(.horizontal, 16)
                            .padding(.vertical, 8)
                            .background(Color.black.opacity(0.7))
                            .cornerRadius(20)
                            .transition(.opacity)
                            .opacity(showToast ? 1 : 0)
                            .animation(.easeInOut(duration: 0.3), value: showToast)
                        Spacer()
                    }
                    .padding(.bottom, 160) // Position higher, overlapping A button
                    Spacer()
                }
                .allowsHitTesting(false) // Prevent interaction with toast
            )
        }
    }
    
    @ViewBuilder
    private var portraitView: some View {
        VStack(spacing: 0) {
            GeometryReader { geometry in
                VStack(spacing: 0) {
                    EmulatorMetalView(delegate: self)
                        .frame(width: geometry.size.width)
                        .frame(height: geometry.size.width * (144.0/160.0))
                    
                    // Toast positioned below the MetalView
                    HStack {
                        Spacer()
                        Text(toastMessage)
                            .foregroundColor(.white)
                            .padding(.horizontal, 16)
                            .padding(.vertical, 8)
                            .background(Color.black.opacity(0.7))
                            .cornerRadius(20)
                            .transition(.opacity)
                            .opacity(showToast ? 1 : 0)
                            .animation(.easeInOut(duration: 0.3), value: showToast)
                        Spacer()
                    }
                    .frame(height: showToast ? 50 : 0) // Reserve space when toast is shown
                    .clipped() // Hide overflow
                }
            }
            VStack(spacing: 50) {
                HStack(spacing: 40) {
                    DirectionalPadView()
                    abButtons
                        .padding(.horizontal, 10)
                        .padding(.vertical, 50)
                }
                selectStartButtons
                    .padding(.bottom, 30)
            }
        }
    }
    
    @ViewBuilder
    private var abButtons: some View {
        HStack(spacing: 30) {
            Button("B") { }
                .frame(width: abButtonSize, height: abButtonSize)
                .background(Color.red)
                .foregroundColor(.white)
                .clipShape(Circle())
                .simultaneousGesture(
                    DragGesture(minimumDistance: 0)
                        .onChanged { _ in
                            impactGenerator.impactOccurred()
                            bridge.buttonB = true;
                        }
                        .onEnded { _ in
                            bridge.buttonB = false;
                        }
                )
            Button("A") { }
                .frame(width: abButtonSize, height: abButtonSize)
                .background(Color.red)
                .foregroundColor(.white)
                .clipShape(Circle())
                .offset(y: -30)
                .simultaneousGesture(
                    DragGesture(minimumDistance: 0)
                        .onChanged { _ in
                            impactGenerator.impactOccurred()
                            bridge.buttonA = true;
                        }
                        .onEnded { _ in
                            bridge.buttonA = false;
                        }
                )
        }
    }

    @ViewBuilder
    private var selectStartButtons : some View {
        HStack(spacing: 30) {
            Button("Select") { }
                .frame(width: 60, height: 30)
                .background(Color.gray)
                .foregroundColor(.white)
                .cornerRadius(15)
                .simultaneousGesture(
                    DragGesture(minimumDistance: 0)
                        .onChanged { _ in
                            impactGenerator.impactOccurred()
                            bridge.buttonSelect = true;
                        }
                        .onEnded { _ in
                            bridge.buttonSelect = false;
                        }
                )
            
            Button("Start") { }
                .frame(width: 60, height: 30)
                .background(Color.gray)
                .foregroundColor(.white)
                .cornerRadius(15)
                .simultaneousGesture(
                    DragGesture(minimumDistance: 0)
                        .onChanged { _ in
                            impactGenerator.impactOccurred()
                            bridge.buttonStart = true;
                        }
                        .onEnded { _ in
                            bridge.buttonStart = false;
                        }
                )
        }
    }
    
    private func showToast(_ message: String) {
        toastMessage = message
        showToast = true
        
        // Hide toast after 2 seconds
        DispatchQueue.main.asyncAfter(deadline: .now() + 2.0) {
            showToast = false
        }
    }
}

// MARK: - EmulatorMetalViewDelegate
extension EmulatorView: EmulatorMetalViewDelegate {
    func didSaveState() {
        bridge.saveState()
        showToast("State Saved")
    }
    
    func didLoadState() {
        bridge.loadPreviouslySavedState()
        showToast("State Loaded")
    }
    
    func didRequestLoadMainState() {
        bridge.loadMainState()
        showToast("Main State Loaded")
    }
    
    func didRequestLoadStateScreen() {
        print("TODO: Handle load state screen")
    }
}

// Preview provider
struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        EmulatorView(romFilename: "pocket.gb")
    }
}
