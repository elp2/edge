import SwiftUI

struct DirectionalPadView: View {
    let minimumDrag: CGFloat = 20
    let viewSize: CGFloat = 175
    var body: some View {
        ZStack {
            Circle()
                .frame(width: viewSize, height: viewSize)
                .foregroundColor(.gray)
                .opacity(0.3)
            
            Rectangle()
                .frame(width: viewSize / 5.0, height:viewSize)
                .foregroundColor(.black)
            Rectangle()
                .frame(width: viewSize, height:viewSize / 5.0)
                .foregroundColor(.black)
        }.gesture(
            DragGesture(minimumDistance: 0)
                .onChanged {
                    drag in
                    let x = drag.location.x + drag.translation.width - viewSize / 2.0
                    let y = drag.location.y + drag.translation.height - viewSize / 2.0
                    
                    if x > minimumDrag {
                        EmulatorBridge.sharedInstance().dpadRight = true
                    } else if abs(x) > minimumDrag {
                        EmulatorBridge.sharedInstance().dpadLeft = true
                    }

                    if y > minimumDrag {
                        EmulatorBridge.sharedInstance().dpadDown = true
                    } else if abs(y) > minimumDrag {
                        EmulatorBridge.sharedInstance().dpadUp = true
                    }
                }
                .onEnded { _ in
                    EmulatorBridge.sharedInstance().dpadUp = false
                    EmulatorBridge.sharedInstance().dpadDown = false
                    EmulatorBridge.sharedInstance().dpadLeft = false
                    EmulatorBridge.sharedInstance().dpadRight = false
                }
        )
    }
}
