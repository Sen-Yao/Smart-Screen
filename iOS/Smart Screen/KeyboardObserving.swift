import SwiftUI
import Combine

final class KeyboardObserving: ObservableObject {
    @Published var keyboardHeight: CGFloat = 0
    private var cancellable: AnyCancellable?

    init() {
        cancellable = NotificationCenter.default.publisher(for: UIResponder.keyboardWillChangeFrameNotification)
            .merge(with: NotificationCenter.default.publisher(for: UIResponder.keyboardWillHideNotification))
            .compactMap { notification -> CGFloat? in
                if let endFrame = notification.userInfo?[UIResponder.keyboardFrameEndUserInfoKey] as? CGRect {
                    return endFrame.height
                } else {
                    return 0
                }
            }
            .assign(to: \.keyboardHeight, on: self)
    }
}
