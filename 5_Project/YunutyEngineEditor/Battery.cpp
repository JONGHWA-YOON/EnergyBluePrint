module htkg.InGame.Battery;
import htkg.InGame;

void Battery::Interact(Robot* invoker, Button* relatedButton)
{
    invoker->battery = this;
    if (remainingBattery == 0)
        invoker->state = Robot::State::BeingDrained;
    else
        invoker->state = Robot::State::Draining;
}
