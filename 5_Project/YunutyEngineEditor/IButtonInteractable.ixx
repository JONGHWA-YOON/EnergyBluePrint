export module htkg.InGame.IButtonInteractable;

export class Robot;
export class Button;

export class IButtonInteractable
{
public:
    virtual void Interact(Robot* invoker, Button* relatedButton)abstract;
};